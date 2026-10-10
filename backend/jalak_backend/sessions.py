"""In-memory session store with ownership, idempotency, and TTL.

Single-process, no database. This is the testable core of API-02/API-03; a
real deployment would need a shared store, but that is not implemented here.
"""

from __future__ import annotations

import time
import uuid
from dataclasses import dataclass, field


class SessionError(Exception):
    """Raised for denied or invalid session operations."""


@dataclass
class Turn:
    turn_id: str
    generation: int
    idempotency_key: str
    status: str = "accepted"  # accepted | processing | done | failed | cancelled
    audio: bytes | None = None
    completed_at_ms: float | None = None
    # Reply metadata, populated when the turn completes. Kept on the turn so an
    # idempotent replay can return the exact same reply.
    reply_text: str | None = None
    action: str | None = None
    book_id: str | None = None


@dataclass
class Session:
    session_id: str
    device_id: str
    created_at_ms: float
    generation: int = 0
    closed: bool = False
    turns: dict[str, Turn] = field(default_factory=dict)
    idempotency_index: dict[str, str] = field(default_factory=dict)  # key -> turn_id


class SessionStore:
    def __init__(self, ttl_ms: int = 600_000, result_expiry_ms: int = 60_000):
        self._sessions: dict[str, Session] = {}
        self._ttl_ms = ttl_ms
        self._result_expiry_ms = result_expiry_ms

    def create(self, device_id: str, now_ms: float | None = None) -> Session:
        now = now_ms if now_ms is not None else time.monotonic() * 1000.0
        sid = uuid.uuid4().hex
        s = Session(session_id=sid, device_id=device_id, created_at_ms=now)
        self._sessions[sid] = s
        return s

    def close(self, session_id: str, device_id: str) -> None:
        s = self._require_owned(session_id, device_id)
        s.closed = True
        s.generation += 1  # invalidate any in-flight results
        for t in s.turns.values():
            if t.status in ("accepted", "processing"):
                t.status = "cancelled"

    def _require_owned(self, session_id: str, device_id: str) -> Session:
        s = self._sessions.get(session_id)
        if s is None:
            raise SessionError("unknown session")
        if s.device_id != device_id:
            raise SessionError("session not owned by this device")
        return s

    def is_expired(self, session: Session, now_ms: float) -> bool:
        return (now_ms - session.created_at_ms) >= self._ttl_ms

    def submit_turn(
        self,
        session_id: str,
        device_id: str,
        idempotency_key: str,
        now_ms: float | None = None,
    ) -> Turn:
        s = self._require_owned(session_id, device_id)
        if s.closed:
            raise SessionError("session closed")
        now = now_ms if now_ms is not None else time.monotonic() * 1000.0
        if self.is_expired(s, now):
            raise SessionError("session expired")

        # Idempotency: a repeated key returns the original turn, never a new one.
        if idempotency_key in s.idempotency_index:
            return s.turns[s.idempotency_index[idempotency_key]]

        tid = uuid.uuid4().hex
        turn = Turn(turn_id=tid, generation=s.generation, idempotency_key=idempotency_key)
        s.turns[tid] = turn
        s.idempotency_index[idempotency_key] = tid
        return turn

    def complete_turn(self, session_id: str, device_id: str, turn_id: str,
                      audio: bytes, now_ms: float | None = None) -> Turn:
        s = self._require_owned(session_id, device_id)
        turn = s.turns.get(turn_id)
        if turn is None:
            raise SessionError("unknown turn")
        if turn.generation != s.generation:
            # A result from before a reset/close must be discarded.
            turn.status = "failed"
            raise SessionError("stale generation")
        now = now_ms if now_ms is not None else time.monotonic() * 1000.0
        turn.status = "done"
        turn.audio = audio
        turn.completed_at_ms = now
        return turn

    def get_turn_audio(self, session_id: str, device_id: str, turn_id: str,
                       now_ms: float | None = None) -> bytes:
        s = self._require_owned(session_id, device_id)
        turn = s.turns.get(turn_id)
        if turn is None:
            raise SessionError("unknown turn")
        if turn.status != "done" or turn.audio is None:
            raise SessionError("audio not ready")
        now = now_ms if now_ms is not None else time.monotonic() * 1000.0
        if turn.completed_at_ms is not None and (now - turn.completed_at_ms) > self._result_expiry_ms:
            raise SessionError("result expired")
        if turn.generation != s.generation:
            raise SessionError("stale generation")
        return turn.audio
