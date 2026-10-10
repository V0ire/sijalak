"""Tests for the runnable entrypoint wiring (no server started)."""

from __future__ import annotations

from jalak_backend.__main__ import build_service, main


def test_build_service_wires_token_and_books():
    svc = build_service(["buku-001", "buku-002"], token="secret")
    assert svc.config.device_token == "secret"
    assert svc.config.book_allowlist == frozenset({"buku-001", "buku-002"})


def test_main_parses_args_without_server(monkeypatch):
    captured = {}

    class FakeUvicorn:
        @staticmethod
        def run(app, host, port, log_level):  # noqa: ARG004
            captured["host"] = host
            captured["port"] = port
            captured["app"] = app

    import sys
    import types

    fake = types.ModuleType("uvicorn")
    fake.run = FakeUvicorn.run
    monkeypatch.setitem(sys.modules, "uvicorn", fake)

    rc = main(["--host", "0.0.0.0", "--port", "9999", "--token", "t", "--book", "b1"])
    assert rc == 0
    assert captured["host"] == "0.0.0.0"
    assert captured["port"] == 9999
