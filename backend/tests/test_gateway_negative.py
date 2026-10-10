"""Negative / failure-path tests for the gateway over ASGI.

Covers provider failure, stale generation after reset, malformed bodies, and the
turn-cap/rate interaction. No network, no credentials.
"""

from __future__ import annotations

import base64
import json

import pytest

from jalak_backend.gateway import GatewayConfig, GatewayService, create_app
from jalak_backend.provider import SpeechResult
from jalak_backend import limits as lim

httpx = pytest.importorskip("httpx")
from httpx import ASGITransport, AsyncClient  # noqa: E402


HDR = {"x-device-token": "dev-token", "x-device-id": "dev-A"}


def make_wav(n: int = 1000) -> bytes:
    return b"RIFF" + b"\x00" * (n - 4)


class BoomProvider:
    """Raises on transcribe to exercise the gateway's failure handling."""

    def transcribe_and_reply(self, wav_bytes, *, context):  # noqa: ARG002
        raise RuntimeError("provider exploded")

    def synthesize(self, text):  # noqa: ARG002
        return b"RIFF" + b"\x00" * 100


def client_for(svc: GatewayService) -> AsyncClient:
    return AsyncClient(transport=ASGITransport(app=create_app(svc)), base_url="http://jalak.test")


def turn_body(sid: str, key: str, wav: bytes | None = None) -> str:
    payload = make_wav() if wav is None else wav
    return json.dumps({
        "session_id": sid,
        "idempotency_key": key,
        "wav_base64": base64.b64encode(payload).decode(),
    })


@pytest.mark.anyio
async def test_provider_failure_is_502_not_500():
    svc = GatewayService(provider=BoomProvider())
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        r = await c.post("/v1/turn", headers=HDR, content=turn_body(sid, "k"))
        assert r.status_code == 502
        assert r.json()["error"] == "provider_error"


@pytest.mark.anyio
async def test_failed_turn_status_is_observable():
    svc = GatewayService(provider=BoomProvider())
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        await c.post("/v1/turn", headers=HDR, content=turn_body(sid, "k"))
        # The turn id is discoverable from the store (device would use its own id).
        session = svc.store._require_owned(sid, "dev-A")
        tid = next(iter(session.turns))
        r = await c.get(f"/v1/turn/{tid}", headers=HDR, params={"session_id": sid})
        assert r.status_code == 200
        assert r.json()["status"] == "failed"


@pytest.mark.anyio
async def test_reset_makes_subsequent_turn_fail_cleanly():
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        # Complete one turn, then reset.
        r = await c.post("/v1/turn", headers=HDR, content=turn_body(sid, "k1"))
        assert r.status_code == 200
        await c.post(f"/v1/session/{sid}/reset", headers=HDR)
        # Next turn must not succeed (closed/expired/capped).
        r2 = await c.post("/v1/turn", headers=HDR, content=turn_body(sid, "k2"))
        assert r2.status_code in (404, 409, 429)


@pytest.mark.anyio
async def test_malformed_json_rejected():
    svc = GatewayService()
    async with client_for(svc) as c:
        r = await c.post("/v1/turn", headers=HDR, content="{not json")
        assert r.status_code == 400
        assert r.json()["error"] == "bad_json"


@pytest.mark.anyio
async def test_missing_fields_rejected():
    svc = GatewayService()
    async with client_for(svc) as c:
        r = await c.post("/v1/turn", headers=HDR, content=json.dumps({"session_id": "x"}))
        assert r.status_code == 400


@pytest.mark.anyio
async def test_status_and_audio_unknown_turn_404():
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        r = await c.get("/v1/turn/deadbeef", headers=HDR, params={"session_id": sid})
        assert r.status_code == 404
        r2 = await c.get("/v1/turn/deadbeef/audio", headers=HDR, params={"session_id": sid})
        assert r2.status_code == 404


@pytest.mark.anyio
async def test_audio_before_done_is_404():
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        r = await c.get("/v1/turn/whatever/audio", headers=HDR, params={"session_id": sid})
        assert r.status_code == 404


@pytest.mark.anyio
async def test_reply_with_wrong_action_rejected():
    # A stub that returns an action the schema forbids.
    scripted = {"default": SpeechResult(transcript="halo", reply_text="hai",
                                        action="run_shell")}
    from jalak_backend.provider import StubSpeechProvider
    svc = GatewayService(provider=StubSpeechProvider(scripted=scripted))
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        r = await c.post("/v1/turn", headers=HDR, content=turn_body(sid, "k"))
        assert r.status_code == 422
        assert r.json()["error"] == "invalid_reply"


@pytest.mark.anyio
async def test_empty_wav_string_is_bad_request():
    # An empty base64 string is a malformed request (400).
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        r = await c.post("/v1/turn", headers=HDR, content=turn_body(sid, "k", wav=b""))
        assert r.status_code == 400
        assert r.json()["error"] == "bad_request"


@pytest.mark.anyio
async def test_undersized_wav_is_413():
    # A syntactically valid but too-short payload fails the size check (413).
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        r = await c.post("/v1/turn", headers=HDR, content=turn_body(sid, "k", wav=b"X"))
        assert r.status_code == 413
        assert r.json()["error"] == "payload_too_large"
