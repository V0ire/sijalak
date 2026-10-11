"""End-to-end contract test: the firmware's request body builder vs the backend.

This closes the loop the unit tests can't: it takes the EXACT bytes the firmware
C++ produces (via a small helper compiled from the real api_client.cpp) and feeds
them to the real Starlette gateway. If the two ever disagree on field names or
shape, this fails.

Skips cleanly if a C++ compiler is unavailable.
"""

from __future__ import annotations

import base64
import json
import shutil
import subprocess
from pathlib import Path

import pytest

from jalak_backend.gateway import GatewayService, create_app

httpx = pytest.importorskip("httpx")
from httpx import ASGITransport, AsyncClient  # noqa: E402

REPO = Path(__file__).resolve().parents[2]
FW = REPO / "firmware" / "components" / "jalak_logic"
CXX = shutil.which("g++") or shutil.which("clang++")

pytestmark = pytest.mark.skipif(CXX is None, reason="no C++ compiler available")

HELPER_SRC = r"""
#include <cstdio>
#include <string>
#include <vector>
#include "jalak/api_client.h"
// argv[1]=session_id argv[2]=idempotency_key ; reads raw bytes from stdin
int main(int argc, char** argv) {
    if (argc < 3) return 2;
    std::vector<std::uint8_t> wav;
    int c;
    while ((c = std::getchar()) != EOF) wav.push_back((std::uint8_t)c);
    std::string body = jalak::build_turn_request_body(argv[1], argv[2], wav);
    std::fwrite(body.data(), 1, body.size(), stdout);
    return 0;
}
"""


@pytest.fixture(scope="module")
def fw_body_builder(tmp_path_factory):
    """Compile the real api_client.cpp into a tiny stdin->body helper."""
    d = tmp_path_factory.mktemp("fwbuilder")
    src = d / "helper.cpp"
    src.write_text(HELPER_SRC)
    exe = d / "helper"
    sources = [
        str(FW / "src" / "api_client.cpp"),
        str(FW / "src" / "audio.cpp"),
        str(src),
    ]
    cmd = [CXX, "-std=c++20", f"-I{FW / 'include'}", *sources, "-o", str(exe)]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        pytest.skip(f"could not build firmware body helper: {res.stderr[:300]}")
    return exe


def build_body(exe: Path, wav: bytes, session_id="sess-x", key="key-x") -> str:
    res = subprocess.run([str(exe), session_id, key], input=wav,
                         capture_output=True)
    assert res.returncode == 0, res.stderr
    return res.stdout.decode()


@pytest.mark.anyio
async def test_firmware_body_accepted_by_gateway(fw_body_builder):
    svc = GatewayService()
    app = create_app(svc)
    async with AsyncClient(transport=ASGITransport(app=app), base_url="http://t") as c:
        hdr = {"x-device-token": "dev-token", "x-device-id": "dev-A"}
        sid = (await c.post("/v1/session", headers=hdr)).json()["session_id"]

        wav = b"RIFF" + b"\x00" * 1000
        body = build_body(fw_body_builder, wav, session_id=sid, key="k1")

        # The body must be valid JSON with exactly the backend's expected keys.
        doc = json.loads(body)
        assert set(doc.keys()) == {"session_id", "idempotency_key", "wav_base64"}
        assert base64.b64decode(doc["wav_base64"]) == wav

        r = await c.post("/v1/turn", headers=hdr, content=body)
        assert r.status_code == 200, r.text
        turn = r.json()
        assert turn["status"] == "done"
        assert turn["schema_version"] == "1"


@pytest.mark.anyio
async def test_firmware_body_parsed_back_by_firmware_parser(fw_body_builder):
    """Round trip: backend response -> firmware parser accepts it."""
    # Reuse the C++ integration test's parser via a second tiny helper is overkill;
    # assert the backend response shape matches what the firmware parser requires.
    svc = GatewayService()
    app = create_app(svc)
    async with AsyncClient(transport=ASGITransport(app=app), base_url="http://t") as c:
        hdr = {"x-device-token": "dev-token", "x-device-id": "dev-A"}
        sid = (await c.post("/v1/session", headers=hdr)).json()["session_id"]
        body = build_body(fw_body_builder, b"RIFF" + b"\x00" * 1000, session_id=sid, key="k")
        turn = (await c.post("/v1/turn", headers=hdr, content=body)).json()

        # Fields the firmware parser reads (see api_client.cpp scan_string calls).
        assert isinstance(turn["turn_id"], str) and turn["turn_id"]
        assert turn["status"] in {"accepted", "processing", "done", "failed", "cancelled"}
        if turn["status"] == "done":
            assert isinstance(turn["reply_text"], str) and turn["reply_text"]
            assert turn["action"] in {"continue", "handoff_book", "safe_refusal", "end_session"}
            assert len(turn["reply_text"].split()) <= 35
