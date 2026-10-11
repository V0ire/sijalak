"""Loopback-only preview: Python routes input, compiled C++ owns state and pixels."""
import argparse
import json
import subprocess
import time
from http.server import BaseHTTPRequestHandler, HTTPServer
from pathlib import Path
from urllib.parse import urlsplit


class Engine:
    def __init__(self, runner, assets=None):
        cmd = [str(runner)] + ([str(assets)] if assets else [])
        self.proc = subprocess.Popen(cmd, stdin=subprocess.PIPE,
                                     stdout=subprocess.PIPE, text=True)
        self.start = time.monotonic_ns()
        self.offset = self.last = 0

    def sample(self, now, down, reduced=False):
        if not 0 <= now < 2**64:
            raise ValueError("clock out of range")
        self.last = now
        self.proc.stdin.write(f"{now} {int(down)} {int(reduced)}\n")
        self.proc.stdin.flush()
        line = self.proc.stdout.readline()
        if not line:
            raise RuntimeError("compiled preview runner stopped; restart server")
        return json.loads(line)

    def request(self, action, reduced=False):
        if action not in {"state", "tap", "double", "hold", "focus-end", "break-end"}:
            raise ValueError("unknown action")
        now = max(self.last, (time.monotonic_ns() - self.start) // 1_000_000 + self.offset)
        sounds = []

        def sample(t, down):
            # Every raw sample can emit a sound; keep them all for this request.
            state = self.sample(t, down, reduced)
            if state.get("sound"):
                sounds.append(state["sound"])
            return state

        sample(now, False)
        if action in {"focus-end", "break-end"}:
            duration = 1_500_000 if action == "focus-end" else 300_000
            self.offset += duration
            now += duration
        elif action != "state":
            # Feed raw levels through the SAME debouncer as firmware, not UI events.
            edges = [(1, True), (31, True)]
            if action == "hold":
                edges += [(1031, True), (1040, False), (1070, False), (1371, False)]
            else:
                edges += [(70, False), (100, False)]
                if action == "double":
                    edges += [(180, True), (210, True), (250, False), (280, False)]
                edges += [(581 if action == "double" else 401, False)]
            for delta, down in edges:
                sample(now + delta, down)
            self.offset += edges[-1][0]
            now += edges[-1][0]
        final = sample(now, False)
        # Report every sound produced by this action (in order), not just the last.
        final["sounds"] = sounds
        return final

    def close(self):
        self.proc.stdin.close()
        try:
            self.proc.wait(timeout=2)
        except subprocess.TimeoutExpired:
            self.proc.terminate()
            self.proc.wait(timeout=2)


def handler(engine, port):
    class Handler(BaseHTTPRequestHandler):
        def send(self, code, content, content_type):
            data = content.encode() if isinstance(content, str) else content
            self.send_response(code)
            self.send_header("Content-Type", content_type)
            self.send_header("Content-Length", str(len(data)))
            self.send_header("Cache-Control", "no-store")
            self.send_header("X-Content-Type-Options", "nosniff")
            self.end_headers()
            self.wfile.write(data)

        def allowed(self):
            hosts = {f"127.0.0.1:{port}", f"localhost:{port}"}
            origin = self.headers.get("Origin")
            return self.headers.get("Host") in hosts and (
                origin is None or origin in {f"http://{h}" for h in hosts})

        def do_GET(self):
            if not self.allowed():
                return self.send(403, "loopback origin required", "text/plain")
            path = urlsplit(self.path).path
            if path == "/":
                return self.send(200, Path(__file__).with_name("index.html").read_bytes(), "text/html; charset=utf-8")
            self.send(404, "not found", "text/plain")

        def do_POST(self):
            if not self.allowed():
                return self.send(403, "loopback origin required", "text/plain")
            if self.path != "/input":
                return self.send(404, "not found", "text/plain")
            try:
                size = int(self.headers.get("Content-Length", "0"))
                if not 1 <= size <= 1024:
                    raise ValueError("request must be 1..1024 bytes")
                doc = json.loads(self.rfile.read(size))
                if not isinstance(doc, dict) or not isinstance(doc.get("action"), str):
                    raise ValueError("action required")
                if not isinstance(doc.get("reduced", False), bool):
                    raise ValueError("reduced must be boolean")
                state = engine.request(doc["action"], doc.get("reduced", False))
                self.send(200, json.dumps(state), "application/json")
            except (ValueError, TypeError) as exc:
                self.send(400, str(exc), "text/plain")
            except (RuntimeError, BrokenPipeError) as exc:
                self.send(503, str(exc), "text/plain")
    return Handler


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--runner", required=True, type=Path)
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--assets", type=Path, default=None,
                        help="local frames dir from tools/mochi_assets.py (optional, not committed)")
    args = parser.parse_args()
    if not args.runner.is_file() or not 1024 <= args.port <= 65535:
        parser.error("runner must exist; port must be 1024..65535")
    assets = args.assets.resolve() if args.assets and args.assets.is_dir() else None
    engine = Engine(args.runner.resolve(), assets)
    try:
        with HTTPServer(("127.0.0.1", args.port), handler(engine, args.port)) as server:
            print(f"Preview lokal: http://127.0.0.1:{args.port} (Ctrl+C berhenti)", flush=True)
            server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        engine.close()


if __name__ == "__main__":
    main()
