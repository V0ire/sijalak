"""Reply schema + validation. The model's output is untrusted; only a validated
reply may drive the device, and `book_id` must be in the curated allowlist.
"""

from __future__ import annotations

from dataclasses import dataclass

ALLOWED_ACTIONS = frozenset(
    {"continue", "handoff_book", "safe_refusal", "end_session"}
)


class ReplyValidationError(Exception):
    pass


@dataclass(frozen=True)
class Reply:
    reply_text: str
    action: str
    book_id: str | None = None
    schema_version: str = "1"


def _count_words(text: str) -> int:
    return len([w for w in text.split() if w.strip()])


def validate_reply(
    raw: dict,
    *,
    book_allowlist: frozenset[str] = frozenset(),
    max_words: int = 35,
) -> Reply:
    """Validate an untrusted provider/model reply.

    Rejects: missing/invalid action, overlong text, unknown book ids, and any
    attempt to smuggle tool/action commands. This is deliberately fail-closed.
    """
    if not isinstance(raw, dict):
        raise ReplyValidationError("reply is not an object")

    text = raw.get("reply_text")
    if not isinstance(text, str) or not text.strip():
        raise ReplyValidationError("reply_text missing or empty")
    if _count_words(text) > max_words:
        raise ReplyValidationError("reply_text exceeds word limit")

    action = raw.get("action")
    if action not in ALLOWED_ACTIONS:
        raise ReplyValidationError(f"action not allowed: {action!r}")

    book_id = raw.get("book_id")
    if book_id is not None:
        if not isinstance(book_id, str):
            raise ReplyValidationError("book_id must be a string or null")
        if action == "handoff_book" and book_id not in book_allowlist:
            raise ReplyValidationError("book_id not in curated allowlist")
        if book_id not in book_allowlist:
            raise ReplyValidationError("book_id not in curated allowlist")

    schema_version = raw.get("schema_version", "1")
    if not isinstance(schema_version, str):
        raise ReplyValidationError("schema_version must be a string")

    return Reply(
        reply_text=text.strip(),
        action=action,
        book_id=book_id,
        schema_version=schema_version,
    )
