"""Wire contract: the single JSON shape shared with the firmware client.

The firmware JSON scanner (firmware/components/jalak_logic/src/api_client.cpp)
reads EXACTLY these fields. Any change here must be mirrored there and in
backend/tests/test_contract_sync.py. Keep it flat and string/number-only: the
device parser is deliberately minimal (no nested objects).

Response shape:
  {
    "schema_version": "1",
    "turn_id": "<hex>",
    "status": "accepted"|"processing"|"done"|"failed"|"cancelled",
    "reply_text": "<str>",        # present when status == done
    "action": "continue"|"handoff_book"|"safe_refusal"|"end_session",
    "book_id": "<str or absent>",
    "audio_bytes": <int>          # present when status == done
  }

Error shape (non-2xx):
  { "error": "<code>", "detail": "<human readable>" }
"""

from __future__ import annotations

import json
from dataclasses import dataclass

SCHEMA_VERSION = "1"
WIRE_CONTENT_TYPE = "application/json"


@dataclass(frozen=True)
class TurnWire:
    turn_id: str
    status: str
    reply_text: str | None = None
    action: str | None = None
    book_id: str | None = None
    audio_bytes: int | None = None

    def to_dict(self) -> dict:
        d: dict = {
            "schema_version": SCHEMA_VERSION,
            "turn_id": self.turn_id,
            "status": self.status,
        }
        if self.reply_text is not None:
            d["reply_text"] = self.reply_text
        if self.action is not None:
            d["action"] = self.action
        if self.book_id is not None:
            d["book_id"] = self.book_id
        if self.audio_bytes is not None:
            d["audio_bytes"] = self.audio_bytes
        return d


def dumps(payload: dict) -> bytes:
    # Compact, no spaces: keeps the device parser and byte caps predictable.
    return json.dumps(payload, separators=(",", ":"), ensure_ascii=False).encode("utf-8")


def error_payload(code: str, detail: str) -> bytes:
    return dumps({"error": code, "detail": detail})
