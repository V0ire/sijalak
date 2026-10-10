"""Runnable entrypoint: `python -m jalak_backend`.

Starts the Starlette gateway under uvicorn with the stub provider (no network,
no credentials). This is the offline E2E server for manual/local testing:

    python -m jalak_backend                 # 127.0.0.1:8787
    python -m jalak_backend --host 0.0.0.0 --port 9000
    python -m jalak_backend --book buku-001 --book buku-002

It is NOT a deployment artifact: no TLS, single process, in-memory sessions, and
a shared-secret token. Production auth/TLS/storage are out of scope and unverified.
"""

from __future__ import annotations

import argparse

from .gateway import GatewayConfig, GatewayService, create_app


def build_service(books: list[str] | None = None, token: str = "dev-token") -> GatewayService:
    cfg = GatewayConfig(device_token=token, book_allowlist=frozenset(books or []))
    return GatewayService(config=cfg)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Si Jalak Bedas offline gateway (stub provider)")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8787)
    parser.add_argument("--token", default="dev-token", help="x-device-token shared secret")
    parser.add_argument("--book", action="append", default=[], help="curated book id (repeatable)")
    args = parser.parse_args(argv)

    try:
        import uvicorn
    except ImportError:
        print("uvicorn is not installed; cannot start the server.")
        print("Install with: pip install uvicorn  (or use the ASGI app directly in tests)")
        return 2

    service = build_service(args.book, args.token)
    app = create_app(service)
    print(f"[jalak] gateway on http://{args.host}:{args.port} (stub provider, token={args.token})")
    uvicorn.run(app, host=args.host, port=args.port, log_level="info")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
