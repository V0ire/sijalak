"""Structural reachability guard; this is not an IDF or electrical safety test.

The temporary local demo MAY use the OLED, the GPIO2 input, the SD card, and the
I2S *speaker* (TX). It MUST NOT reach the microphone, any network/cloud path, or
the legacy voice entrypoint. Kept as a cheap source-level guard only.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def check_entrypoint(source):
    text = Path(source).read_text()
    text = re.sub(r"//[^\n]*|/\*.*?\*/", "", text, flags=re.S)
    # Still- forbidden: any microphone capture, network, or legacy voice path.
    forbidden = r"wifi|WiFi|esp_http|EspHttp|DeviceApiClient|start_capture|stop_capture|legacy_voice|create_session|submit_turn"
    assert not re.search(forbidden, text), "default activates mic, network, or legacy voice"
    assert 'extern "C" void app_main' in text
    assert 'jalak/local/demo.h' in text
    # The mic pins must stay disabled (-1) in the local demo.
    assert "-1, -1, -1" in text, "mic RX pins must be disabled in local demo"

if __name__ == "__main__":
    check_entrypoint(sys.argv[1] if len(sys.argv) > 1 else ROOT / "firmware/components/jalak_app/src/app_main.cpp")
    app = (ROOT / "firmware/components/jalak_app/CMakeLists.txt").read_text()
    hal = (ROOT / "firmware/components/jalak_hal/CMakeLists.txt").read_text()
    # Legacy voice + the cloud transport stay out of the default build.
    assert '"src/legacy_voice.cpp"' not in app
    assert '"src/esp_http_transport.cpp"' not in hal
    # The local demo's allowed drivers ARE present (speaker + SD, not network).
    assert '"src/i2s_audio_device.cpp"' in hal
    assert '"src/sd_card.cpp"' in hal
    logic = (ROOT / "firmware/components/jalak_logic/CMakeLists.txt").read_text()
    assert '"src/api_client.cpp"' not in logic, "unused voice API must stay outside local IDF build"
    # Public/private dependencies must not reconnect a network path either.
    manifests = re.sub(r"#[^\n]*", "", app + hal + logic)
    assert not re.search(r"esp_wifi|esp_http|esp_netif|legacy_voice", manifests)
    print("local-only source/component guards passed (NOT target/hardware validation)")
