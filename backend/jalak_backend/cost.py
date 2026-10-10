"""Cost admission. Fail closed when pricing or budget is not configured, so an
unconfigured environment can never silently spend money or assume a free tier.
"""

from __future__ import annotations

from dataclasses import dataclass, field


@dataclass(frozen=True)
class CostPolicy:
    # All fields must be explicitly configured. None = unknown = deny.
    monthly_cap_idr: int | None = None
    stt_price_per_minute_idr: float | None = None
    chat_price_per_1k_tokens_idr: float | None = None
    tts_price_per_1k_chars_idr: float | None = None
    billing_authorized: bool = False


@dataclass
class CostAdmission:
    allowed: bool
    reason: str = "ok"
    estimated_idr: float = 0.0


def _configured(policy: CostPolicy) -> bool:
    return all(
        v is not None
        for v in (
            policy.monthly_cap_idr,
            policy.stt_price_per_minute_idr,
            policy.chat_price_per_1k_tokens_idr,
            policy.tts_price_per_1k_chars_idr,
        )
    )


def admit_provider_call(
    policy: CostPolicy,
    *,
    spent_idr_this_month: float,
    est_audio_seconds: float,
    est_chat_tokens: int,
    est_tts_chars: int,
) -> CostAdmission:
    if not policy.billing_authorized:
        return CostAdmission(False, "billing not authorized")
    if not _configured(policy):
        return CostAdmission(False, "pricing/account unknown (fail closed)")

    est = (
        (est_audio_seconds / 60.0) * policy.stt_price_per_minute_idr
        + (est_chat_tokens / 1000.0) * policy.chat_price_per_1k_tokens_idr
        + (est_tts_chars / 1000.0) * policy.tts_price_per_1k_chars_idr
    )
    if spent_idr_this_month + est > policy.monthly_cap_idr:
        return CostAdmission(False, "monthly cap would be exceeded", est)
    return CostAdmission(True, "ok", est)
