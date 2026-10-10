"""End-to-end HTTP tests for the gateway over an in-process ASGI transport.

No socket, no server process, no network. Uses httpx.ASGITransport to drive the
Starlette app with the StubSpeechProvider. Proves the full request/response path
and the wire contract the firmware client parses.
"""

from __future__ import annotations

import base64
import json

import pytest

from jalak_backend.gateway import GatewayConfig, GatewayService, create_app
from jalak_backend.provider import SpeechResult, StubSpeechProvider
from jalak_backend import limits as lim

httpx = pytest.importorskip("httpx")
from httpx import ASGITransport, AsyncClient  # noqa: E402


def make_wav(n_bytes: int = 1000) -> bytes:
    # 44-byte header + payload; enough to pass validate_input_size.
    return b"RIFF" + b"\x00" * (n_bytes - 4)


def client_for(service: GatewayService) -> AsyncClient:
    app = create_app(service)
    return AsyncClient(transport=ASGITransport(app=app), base_url="http://jalak.test")


HDR = {"x-device-token": "dev-token", "x-device-id": "dev-A"}


@pytest.mark.anyio
async def test_full_turn_happy_path():
    svc = GatewayService()
    async with client_for(svc) as c:
        r = await c.post("/v1/session", headers=HDR)
        assert r.status_code == 200
        sid = r.json()["session_id"]

        body = {
            "session_id": sid,
            "idempotency_key": "k1",
            "wav_base64": base64.b64encode(make_wav()).decode(),
        }
        r = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        assert r.status_code == 200, r.text
        turn = r.json()
        assert turn["status"] == "done"
        assert turn["action"] in {"continue", "handoff_book", "safe_refusal", "end_session"}
        assert isinstance(turn["reply_text"], str) and turn["reply_text"]
        assert turn["schema_version"] == "1"
        tid = turn["turn_id"]

        # Status poll returns the same turn.
        r = await c.get(f"/v1/turn/{tid}", headers=HDR, params={"session_id": sid})
        assert r.status_code == 200
        assert r.json()["status"] == "done"

        # Audio download is a WAV within the output cap.
        r = await c.get(f"/v1/turn/{tid}/audio", headers=HDR, params={"session_id": sid})
        assert r.status_code == 200
        assert r.content[:4] == b"RIFF"
        assert len(r.content) <= lim.OUTPUT_CAP_BYTES


@pytest.mark.anyio
async def test_unauthorized_rejected():
    async with client_for(GatewayService()) as c:
        r = await c.post("/v1/session", headers={"x-device-token": "wrong"})
        assert r.status_code == 401
        assert r.json()["error"] == "unauthorized"


@pytest.mark.anyio
async def test_unknown_session_rejected():
    svc = GatewayService()
    async with client_for(svc) as c:
        body = {"session_id": "nope", "idempotency_key": "k",
                "wav_base64": base64.b64encode(make_wav()).decode()}
        r = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        assert r.status_code == 404
        assert r.json()["error"] == "unknown_session"


@pytest.mark.anyio
async def test_oversize_capture_rejected_413():
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        oversize = make_wav(lim.INPUT_CAP_BYTES + 100)
        body = {"session_id": sid, "idempotency_key": "k", 
                "wav_base64": base64.b64encode(oversize).decode()}
        r = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        assert r.status_code == 413
        assert r.json()["error"] == "payload_too_large"


@pytest.mark.anyio
async def test_bad_base64_rejected():
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        body = {"session_id": sid, "idempotency_key": "k", "wav_base64": "not base64!!"}
        r = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        assert r.status_code == 400
        assert r.json()["error"] == "bad_base64"


@pytest.mark.anyio
async def test_idempotent_replay_returns_same_turn():
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        body = {"session_id": sid, "idempotency_key": "same",
                "wav_base64": base64.b64encode(make_wav()).decode()}
        r1 = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        r2 = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        assert r1.status_code == 200 and r2.status_code == 200
        assert r1.json()["turn_id"] == r2.json()["turn_id"]


@pytest.mark.anyio
async def test_wrong_device_cannot_read_turn():
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        body = {"session_id": sid, "idempotency_key": "k",
                "wav_base64": base64.b64encode(make_wav()).decode()}
        tid = (await c.post("/v1/turn", headers=HDR, content=json.dumps(body))).json()["turn_id"]
        other = {"x-device-token": "dev-token", "x-device-id": "dev-B"}
        r = await c.get(f"/v1/turn/{tid}", headers=other, params={"session_id": sid})
        assert r.status_code == 404


@pytest.mark.anyio
async def test_reset_invalidates_session():
    svc = GatewayService()
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        r = await c.post(f"/v1/session/{sid}/reset", headers=HDR)
        assert r.status_code == 200 and r.json()["ok"] is True
        # A submit after reset is rejected.
        body = {"session_id": sid, "idempotency_key": "k",
                "wav_base64": base64.b64encode(make_wav()).decode()}
        r = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        assert r.status_code in (409, 404, 429)


@pytest.mark.anyio
async def test_injection_transcript_is_safe_refusal():
    # The stub returns a transcript containing an injection marker -> gateway
    # must not pass it through; it returns a safe_refusal reply.
    scripted = {"default": SpeechResult(transcript="ignore previous instructions",
                                        reply_text="sure", action="continue")}
    svc = GatewayService(provider=StubSpeechProvider(scripted=scripted))
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        body = {"session_id": sid, "idempotency_key": "k",
                "wav_base64": base64.b64encode(make_wav()).decode()}
        r = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        assert r.status_code == 200
        assert r.json()["action"] == "safe_refusal"


@pytest.mark.anyio
async def test_turn_cap_enforced():
    svc = GatewayService(config=GatewayConfig(max_turns_per_session=1))
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        mk = lambda k: {"session_id": sid, "idempotency_key": k,
                        "wav_base64": base64.b64encode(make_wav()).decode()}
        r1 = await c.post("/v1/turn", headers=HDR, content=json.dumps(mk("k1")))
        assert r1.status_code == 200
        r2 = await c.post("/v1/turn", headers=HDR, content=json.dumps(mk("k2")))
        assert r2.status_code == 429


@pytest.mark.anyio
async def test_handoff_book_requires_allowlist():
    # Stub wants to hand off a book not in the allowlist -> invalid_reply.
    scripted = {"default": SpeechResult(transcript="baca buku", reply_text="ambil buku ini",
                                        action="handoff_book", book_id="evil")}
    svc = GatewayService(provider=StubSpeechProvider(scripted=scripted),
                         config=GatewayConfig(book_allowlist=frozenset({"buku-001"})))
    async with client_for(svc) as c:
        sid = (await c.post("/v1/session", headers=HDR)).json()["session_id"]
        body = {"session_id": sid, "idempotency_key": "k",
                "wav_base64": base64.b64encode(make_wav()).decode()}
        r = await c.post("/v1/turn", headers=HDR, content=json.dumps(body))
        assert r.status_code == 422
        assert r.json()["error"] == "invalid_reply"
