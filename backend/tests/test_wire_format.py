"""Lock the wire format emitted by wire.py.

The firmware integration test (firmware/test/test_integration.cpp) hardcodes the
exact strings this module produces. If either side changes, this test fails,
prompting an update of both. Keep the field set and ordering stable.
"""

from __future__ import annotations

from jalak_backend.wire import SCHEMA_VERSION, TurnWire, dumps, error_payload


def test_done_turn_wire_shape():
    w = TurnWire(turn_id="abc123", status="done", reply_text="Mari kita baca bersama",
                 action="continue", audio_bytes=48044)
    s = dumps(w.to_dict()).decode()
    assert s == (
        '{"schema_version":"1","turn_id":"abc123","status":"done",'
        '"reply_text":"Mari kita baca bersama","action":"continue","audio_bytes":48044}'
    )


def test_handoff_wire_shape():
    w = TurnWire(turn_id="abc123", status="done", reply_text="Ambil buku ini",
                 action="handoff_book", book_id="buku-001", audio_bytes=24044)
    s = dumps(w.to_dict()).decode()
    assert s == (
        '{"schema_version":"1","turn_id":"abc123","status":"done",'
        '"reply_text":"Ambil buku ini","action":"handoff_book",'
        '"book_id":"buku-001","audio_bytes":24044}'
    )


def test_status_wire_shape():
    w = TurnWire(turn_id="abc123", status="processing")
    assert dumps(w.to_dict()).decode() == (
        '{"schema_version":"1","turn_id":"abc123","status":"processing"}'
    )


def test_error_wire_shape():
    assert error_payload("payload_too_large", "capture exceeds 8 s canonical cap").decode() == (
        '{"error":"payload_too_large","detail":"capture exceeds 8 s canonical cap"}'
    )


def test_no_spaces_and_ascii_unicode_preserved():
    # Compact separators; non-ASCII kept as UTF-8 (firmware decodes UTF-8).
    w = TurnWire(turn_id="t", status="done", reply_text="café — ayo", action="continue")
    raw = dumps(w.to_dict())
    assert b", " not in raw and b": " not in raw
    assert "café".encode("utf-8") in raw


def test_schema_version_constant():
    assert SCHEMA_VERSION == "1"
