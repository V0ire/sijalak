"""User-text guardrails: cheap first-pass checks before any provider call.

This is not a content-safety guarantee and does not replace model-side policy.
It fails closed on obviously disallowed patterns so the demo can show bounded
behavior without a live model.
"""

from __future__ import annotations

from dataclasses import dataclass

# Instruction-override style attempts. Intentionally conservative and small.
_INJECTION_MARKERS = (
    "ignore previous",
    "ignore all previous",
    "disregard the system",
    "you are now",
    "system prompt",
    "reveal your instructions",
    "abaikan instruksi",
    "lupakan aturan",
)

_HARM_MARKERS = (
    "cara menyakiti",
    "how to hurt",
    "cara membunuh",
)


@dataclass(frozen=True)
class GuardrailResult:
    allow: bool
    reason: str = "ok"
    # When not allowed, this is the action the controller should take.
    suggested_action: str = "continue"


def inspect_user_text(text: str, *, max_words: int = 60) -> GuardrailResult:
    if not isinstance(text, str) or not text.strip():
        return GuardrailResult(False, "empty input", "end_session")

    lowered = text.lower()
    for marker in _INJECTION_MARKERS:
        if marker in lowered:
            return GuardrailResult(False, "prompt-injection marker", "safe_refusal")
    for marker in _HARM_MARKERS:
        if marker in lowered:
            return GuardrailResult(False, "harmful request marker", "safe_refusal")

    if len([w for w in text.split() if w.strip()]) > max_words:
        return GuardrailResult(False, "input too long", "safe_refusal")

    return GuardrailResult(True, "ok", "continue")
