"""Provider interface + stub. The stub produces deterministic, clearly-labelled
canned replies so the whole pipeline can be tested with zero credentials and
zero network. A real provider adapter is NOT implemented in this session.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Protocol


@dataclass(frozen=True)
class SpeechResult:
    transcript: str
    reply_text: str
    action: str  # continue | handoff_book | safe_refusal | end_session
    book_id: str | None = None
    is_stub: bool = True


class SpeechProvider(Protocol):
    """Remaining roles: STT -> dialogue -> TTS. Implementations may split these
    into separate calls; the interface keeps them behind one entry point."""

    def transcribe_and_reply(self, wav_bytes: bytes, *, context: dict) -> SpeechResult:
        ...

    def synthesize(self, reply_text: str) -> bytes:
        ...


class StubSpeechProvider:
    """Deterministic offline stand-in. Never calls a network or a model."""

    def __init__(self, *, scripted: dict[str, SpeechResult] | None = None):
        self._scripted = scripted or {}

    def transcribe_and_reply(self, wav_bytes: bytes, *, context: dict) -> SpeechResult:
        if not wav_bytes:
            return SpeechResult(
                transcript="",
                reply_text="Aku tidak mendengar apa-apa. Coba lagi?",
                action="continue",
            )
        key = context.get("script_key", "default")
        if key in self._scripted:
            return self._scripted[key]
        return SpeechResult(
            transcript="(stub transcript)",
            reply_text="Ini respons contoh dari mode uji.",
            action="continue",
        )

    def synthesize(self, reply_text: str) -> bytes:
        # Fake but size-realistic WAV: 44-byte header + 24 kHz 16-bit silence.
        # Clearly a stub, never played to a real user as if it were a model voice.
        seconds = min(15, max(1, len(reply_text) // 12))
        samples = 24_000 * seconds
        payload = b"\x00\x00" * samples
        header = b"RIFF" + (36 + len(payload)).to_bytes(4, "little") + b"WAVE"
        header += b"fmt " + (16).to_bytes(4, "little") + (1).to_bytes(2, "little")
        header += (1).to_bytes(2, "little") + (24_000).to_bytes(4, "little")
        header += (48_000).to_bytes(4, "little") + (2).to_bytes(2, "little")
        header += (16).to_bytes(2, "little") + b"data" + len(payload).to_bytes(4, "little")
        return header + payload
