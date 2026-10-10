"""Starlette HTTP gateway for Si Jalak Bedas.

Why Starlette, not FastAPI: this environment has starlette/uvicorn/pydantic
installed but NOT fastapi. Building directly on Starlette keeps the whole gateway
offline-installable and testable with no new dependency, while preserving the
route/response contract. A FastAPI wrapper can be added later without changing
routes (FastAPI subclasses Starlette).

Routes (namespace /v1, matching the device client + .planning/ARCHITECTURE.md):
  POST /v1/session                 register a device session -> {session_id}
  POST /v1/turn                    submit a turn (JSON body, base64 WAV) -> TurnWire
  GET  /v1/turn/{turn_id}          status poll -> TurnWire (status only)
  GET  /v1/turn/{turn_id}/audio    download canonical WAV (<= output cap)
  POST /v1/session/{sid}/reset     close + invalidate (STOP) -> {ok:true}

Auth: every route requires header `x-device-token` equal to the configured token.
This is a shared-secret placeholder for the isolated adult demo, NOT a product
auth design. Real auth is out of scope and unverified.

The provider is injected. Offline tests use StubSpeechProvider (no network). Any
real provider adapter remains UNVERIFIED and is not implemented here.
"""

from __future__ import annotations

import base64
import binascii
import json
import time
from dataclasses import dataclass, field

from starlette.applications import Starlette
from starlette.requests import Request
from starlette.responses import Response
from starlette.routing import Route

from . import limits as lim
from .guardrails import inspect_user_text
from .provider import SpeechProvider, StubSpeechProvider
from .reply_schema import ReplyValidationError, validate_reply
from .sessions import SessionError, SessionStore, Turn
from .wire import TurnWire, WIRE_CONTENT_TYPE, dumps, error_payload


@dataclass
class GatewayConfig:
    device_token: str = "dev-token"
    book_allowlist: frozenset[str] = frozenset()
    limits: lim.Limits = field(default_factory=lim.Limits)
    max_turns_per_session: int = 4


class GatewayService:
    """Holds gateway state and the turn pipeline. No framework imports, so it is
    unit-testable without HTTP."""

    def __init__(
        self,
        *,
        config: GatewayConfig | None = None,
        provider: SpeechProvider | None = None,
        store: SessionStore | None = None,
    ) -> None:
        self.config = config or GatewayConfig()
        self.provider = provider or StubSpeechProvider()
        self.store = store or SessionStore(
            ttl_ms=self.config.limits.session_ttl_ms,
            result_expiry_ms=self.config.limits.result_expiry_ms,
        )

    # ---- session -----------------------------------------------------------

    def create_session(self, device_id: str) -> dict:
        s = self.store.create(device_id)
        return {"session_id": s.session_id}

    def reset_session(self, session_id: str, device_id: str) -> dict:
        self.store.close(session_id, device_id)
        return {"ok": True}

    # ---- turn --------------------------------------------------------------

    def submit_turn(
        self,
        *,
        session_id: str,
        device_id: str,
        idempotency_key: str,
        wav_bytes: bytes,
        turns_in_window: int = 0,
    ) -> tuple[int, dict]:
        """Returns (http_status, payload dict)."""
        size = lim.validate_input_size(len(wav_bytes))
        if not size.allowed:
            return 413, {"error": "payload_too_large", "detail": size.reason}

        try:
            session = self.store._require_owned(session_id, device_id)
        except SessionError as e:
            return 404, {"error": "unknown_session", "detail": str(e)}

        if len(session.turns) >= self.config.max_turns_per_session:
            return 429, {"error": "turn_cap", "detail": "turn cap reached"}

        admit = lim.admit_turn(
            turn_count=len(session.turns),
            session_elapsed_ms=max(0, int(time.monotonic() * 1000 - session.created_at_ms)),
            new_turns_in_window=turns_in_window,
            limits=self.config.limits,
        )
        if not admit.allowed:
            return 429, {"error": "admission_denied", "detail": admit.reason}

        try:
            turn = self.store.submit_turn(session_id, device_id, idempotency_key)
        except SessionError as e:
            return 409, {"error": "session_error", "detail": str(e)}

        # Idempotent replay: a repeated key returns the already-produced turn.
        if turn.status == "done" and turn.reply_text is not None:
            return 200, self._done_wire(turn).to_dict()

        try:
            result = self.provider.transcribe_and_reply(wav_bytes, context={"session_id": session_id})
        except Exception as e:  # provider failure -> failed turn, never a 500 leak
            turn.status = "failed"
            return 502, {"error": "provider_error", "detail": type(e).__name__}

        # Guardrail on the transcript (fail-closed, cheap first pass).
        guard = inspect_user_text(result.transcript)
        if not guard.allow:
            return self._complete(turn, session_id, device_id,
                                  reply_text="Maaf, aku belum bisa membantu itu. Yuk lanjut baca?",
                                  action="safe_refusal", book_id=None)

        try:
            reply = validate_reply(
                {
                    "reply_text": result.reply_text,
                    "action": result.action,
                    "book_id": result.book_id,
                },
                book_allowlist=self.config.book_allowlist,
                max_words=self.config.limits.max_reply_words,
            )
        except ReplyValidationError as e:
            turn.status = "failed"
            return 422, {"error": "invalid_reply", "detail": str(e)}

        return self._complete(turn, session_id, device_id,
                              reply_text=reply.reply_text, action=reply.action,
                              book_id=reply.book_id)

    def _complete(self, turn: Turn, session_id: str, device_id: str, *,
                  reply_text: str, action: str, book_id: str | None) -> tuple[int, dict]:
        audio = self.provider.synthesize(reply_text)
        if len(audio) > lim.OUTPUT_CAP_BYTES:
            turn.status = "failed"
            return 502, {"error": "output_too_large", "detail": "synthesized audio exceeds cap"}
        turn.reply_text = reply_text
        turn.action = action
        turn.book_id = book_id
        self.store.complete_turn(session_id, device_id, turn.turn_id, audio)
        return 200, self._done_wire(turn).to_dict()

    def _done_wire(self, turn: Turn) -> TurnWire:
        return TurnWire(
            turn_id=turn.turn_id,
            status="done",
            reply_text=turn.reply_text,
            action=turn.action,
            book_id=turn.book_id,
            audio_bytes=len(turn.audio) if turn.audio else 0,
        )

    # ---- status / audio ----------------------------------------------------

    def get_status(self, session_id: str, device_id: str, turn_id: str) -> tuple[int, dict]:
        try:
            session = self.store._require_owned(session_id, device_id)
        except SessionError as e:
            return 404, {"error": "unknown_session", "detail": str(e)}
        turn = session.turns.get(turn_id)
        if turn is None:
            return 404, {"error": "unknown_turn", "detail": "no such turn"}
        return 200, TurnWire(turn_id=turn_id, status=turn.status).to_dict()

    def get_audio(self, session_id: str, device_id: str, turn_id: str) -> tuple[int, bytes]:
        try:
            audio = self.store.get_turn_audio(session_id, device_id, turn_id)
        except SessionError:
            return 404, b""
        return 200, audio


# ---------------------------------------------------------------------------
# Starlette wiring. Thin: parse/authorize/validate transport, delegate to
# GatewayService, encode the wire response.
# ---------------------------------------------------------------------------


def _json(payload: dict, status: int = 200) -> Response:
    return Response(dumps(payload), status_code=status, media_type=WIRE_CONTENT_TYPE)


def _err(code: str, detail: str, status: int) -> Response:
    return Response(error_payload(code, detail), status_code=status,
                    media_type=WIRE_CONTENT_TYPE)


def create_app(service: GatewayService | None = None) -> Starlette:
    svc = service or GatewayService()

    def _authorized(request: Request) -> bool:
        return request.headers.get("x-device-token") == svc.config.device_token

    def _device_id(request: Request) -> str:
        return request.headers.get("x-device-id", "unknown-device")

    async def create_session(request: Request) -> Response:
        if not _authorized(request):
            return _err("unauthorized", "bad or missing device token", 401)
        return _json(svc.create_session(_device_id(request)))

    async def reset_session(request: Request) -> Response:
        if not _authorized(request):
            return _err("unauthorized", "bad or missing device token", 401)
        sid = request.path_params["session_id"]
        try:
            return _json(svc.reset_session(sid, _device_id(request)))
        except SessionError as e:
            return _err("unknown_session", str(e), 404)

    async def submit_turn(request: Request) -> Response:
        if not _authorized(request):
            return _err("unauthorized", "bad or missing device token", 401)
        try:
            body = await request.body()
        except Exception:
            return _err("bad_request", "unreadable body", 400)
        if len(body) > lim.INPUT_CAP_BYTES * 2:  # JSON+base64 ~1.34x of raw
            return _err("payload_too_large", "request body exceeds transport cap", 413)
        try:
            doc = json.loads(body)
        except (json.JSONDecodeError, UnicodeDecodeError):
            return _err("bad_json", "request body is not valid JSON", 400)
        if not isinstance(doc, dict):
            return _err("bad_json", "request body must be an object", 400)

        session_id = doc.get("session_id")
        idem = doc.get("idempotency_key")
        wav_b64 = doc.get("wav_base64")
        if not isinstance(session_id, str) or not session_id:
            return _err("bad_request", "session_id required", 400)
        if not isinstance(idem, str) or not idem:
            return _err("bad_request", "idempotency_key required", 400)
        if not isinstance(wav_b64, str) or not wav_b64:
            return _err("bad_request", "wav_base64 required", 400)
        try:
            wav = base64.b64decode(wav_b64, validate=True)
        except (binascii.Error, ValueError):
            return _err("bad_base64", "wav_base64 is not valid base64", 400)

        turns_in_window = 0
        try:
            turns_in_window = int(request.headers.get("x-turns-in-window", "0"))
        except ValueError:
            turns_in_window = 0

        status, payload = svc.submit_turn(
            session_id=session_id, device_id=_device_id(request),
            idempotency_key=idem, wav_bytes=wav, turns_in_window=turns_in_window,
        )
        return _json(payload, status)

    async def turn_status(request: Request) -> Response:
        if not _authorized(request):
            return _err("unauthorized", "bad or missing device token", 401)
        sid = request.query_params.get("session_id", "")
        tid = request.path_params["turn_id"]
        status, payload = svc.get_status(sid, _device_id(request), tid)
        return _json(payload, status)

    async def turn_audio(request: Request) -> Response:
        if not _authorized(request):
            return _err("unauthorized", "bad or missing device token", 401)
        sid = request.query_params.get("session_id", "")
        tid = request.path_params["turn_id"]
        status, audio = svc.get_audio(sid, _device_id(request), tid)
        if status != 200:
            return _err("no_audio", "audio not available", status)
        return Response(audio, status_code=200, media_type="audio/wav")

    routes = [
        Route("/v1/session", create_session, methods=["POST"]),
        Route("/v1/session/{session_id}/reset", reset_session, methods=["POST"]),
        Route("/v1/turn", submit_turn, methods=["POST"]),
        Route("/v1/turn/{turn_id}", turn_status, methods=["GET"]),
        Route("/v1/turn/{turn_id}/audio", turn_audio, methods=["GET"]),
    ]
    return Starlette(routes=routes)
