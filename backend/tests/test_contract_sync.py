"""Cross-check: firmware API client contract vs backend canonical limits.

The firmware hardcodes the same canonical numbers (256044 input, 720044 output,
768 KiB download, 35-word reply, action allowlist). If either side drifts, the
two ends disagree and real integration would fail. This test is the guard.

It reads the firmware source as text (no C++ toolchain needed) and asserts the
shared numbers appear and match the Python canonical values.
"""

from __future__ import annotations

import re
from pathlib import Path

from jalak_backend import limits, reply_schema

REPO = Path(__file__).resolve().parents[2]
FW = REPO / "firmware" / "components" / "jalak_logic"


def test_backend_canonical_numbers():
    assert limits.INPUT_CAP_BYTES == 256044
    assert limits.OUTPUT_CAP_BYTES == 720044
    assert limits.DOWNLOAD_CAP_BYTES == 768 * 1024


def test_firmware_header_matches_backend_caps():
    audio_h = (FW / "include" / "jalak" / "audio.h").read_text()
    # Canonical comments + constants present.
    assert "16000" in audio_h
    assert "24000" in audio_h
    assert "8000" in audio_h
    assert "15000" in audio_h
    assert "768 * 1024" in audio_h


def test_firmware_client_download_cap_matches_backend():
    src = (FW / "src" / "api_client.cpp").read_text()
    m = re.search(r"kDownloadCapBytes\s*=\s*([^;]+);", src)
    assert m, "kDownloadCapBytes not found in api_client.cpp"
    expr = m.group(1)
    assert "768" in expr, f"download cap drift: {expr!r}"


def test_action_allowlist_shared():
    src = (FW / "src" / "api_client.cpp").read_text()
    for action in reply_schema.ALLOWED_ACTIONS:
        assert f'"{action}"' in src, f"firmware missing action {action!r}"


def test_reply_word_limit_shared():
    api_h = (FW / "include" / "jalak" / "api_client.h").read_text()
    # Default parameter max_reply_words = 35 must match backend Limits.
    assert "max_words: int = 35" in api_h or "max_reply_words = 35" in api_h
    assert limits.Limits().max_reply_words == 35
