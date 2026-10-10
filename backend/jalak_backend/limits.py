"""Canonical bounds and admission logic.

These mirror the proposed contract in .planning/ARCHITECTURE.md. They are
proposals, not observed provider behavior. Tests assert the arithmetic and the
fail-closed rules, nothing more.
"""

from __future__ import annotations

from dataclasses import dataclass

# Canonical audio limits (proposed).
INPUT_MAX_MS = 8000
OUTPUT_MAX_MS = 15000
INPUT_SAMPLE_RATE = 16000
OUTPUT_SAMPLE_RATE = 24000
BYTES_PER_SAMPLE = 2  # 16-bit mono
WAV_HEADER_BYTES = 44
DOWNLOAD_CAP_BYTES = 768 * 1024  # 768 KiB


def pcm_bytes_for_ms(sample_rate: int, ms: int) -> int:
    return sample_rate * BYTES_PER_SAMPLE * ms // 1000


def canonical_wav_bytes(pcm_bytes: int) -> int:
    return pcm_bytes + WAV_HEADER_BYTES


INPUT_CAP_BYTES = canonical_wav_bytes(pcm_bytes_for_ms(INPUT_SAMPLE_RATE, INPUT_MAX_MS))
OUTPUT_CAP_BYTES = canonical_wav_bytes(pcm_bytes_for_ms(OUTPUT_SAMPLE_RATE, OUTPUT_MAX_MS))


@dataclass(frozen=True)
class Limits:
    max_turns: int = 4
    max_session_ms: int = 180_000
    max_reply_words: int = 35
    new_turns_per_minute: int = 5
    status_requests_per_second: int = 3
    session_ttl_ms: int = 600_000  # 10 min
    result_expiry_ms: int = 60_000  # 60 s after completion


@dataclass(frozen=True)
class Admission:
    allowed: bool
    reason: str = "ok"


def admit_turn(
    *,
    turn_count: int,
    session_elapsed_ms: int,
    new_turns_in_window: int,
    limits: Limits = Limits(),
) -> Admission:
    """Decide whether a NEW turn may start. Fail closed with a reason."""
    if turn_count >= limits.max_turns:
        return Admission(False, "turn cap reached")
    if session_elapsed_ms >= limits.max_session_ms:
        return Admission(False, "session time cap reached")
    if new_turns_in_window >= limits.new_turns_per_minute:
        return Admission(False, "new-turn rate cap reached")
    return Admission(True)


def admit_status_poll(*, status_requests_in_second: int, limits: Limits = Limits()) -> Admission:
    """Status polling is capped separately from paid new turns, so 500 ms polling
    is not mistaken for a paid inference."""
    if status_requests_in_second >= limits.status_requests_per_second:
        return Admission(False, "status poll rate cap reached")
    return Admission(True)


def validate_input_size(bytes_len: int) -> Admission:
    if bytes_len == 0:
        return Admission(False, "empty capture")
    if bytes_len <= WAV_HEADER_BYTES:
        return Admission(False, "shorter than a WAV header")
    if bytes_len > INPUT_CAP_BYTES:
        return Admission(False, "capture exceeds 8 s canonical cap")
    return Admission(True)
