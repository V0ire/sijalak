// IDF I2S audio driver. Compiles only under ESP-IDF. UNVERIFIED.
// Half-duplex is enforced here: you cannot capture while playing, matching the
// portable rule in jalak_logic and the mock in mock_audio.h.

#if defined(ESP_PLATFORM)

#include "jalak/hal/i2s_audio_device.h"

#include "driver/i2s_std.h"
#include "esp_log.h"
#include "esp_timer.h"

namespace jalak::hal {

namespace {
constexpr char kTag[] = "jalak_i2s";
constexpr int kCaptureTimeoutMs = 100;

// Take at most the canonical input cap of PCM from the mic.
std::size_t max_capture_pcm_bytes() {
    return jalak::pcm_bytes_for_ms(16000, 2, 8000);
}
}  // namespace

bool I2sAudioDevice::begin(const I2sPins& pins) {
    // Log the intended wiring so a bench bring-up can confirm it, but do NOT
    // pass it to the IDF i2s driver yet: the real channel/handle setup is
    // UNVERIFIED and must be validated on hardware first.
    ESP_LOGW(kTag,
             "I2sAudioDevice::begin stub — TX(bclk=%d ws=%d din=%d) "
             "RX(bclk=%d ws=%d dout=%d); handle setup UNVERIFIED",
             pins.tx_bclk, pins.tx_ws, pins.tx_din,
             pins.rx_bclk, pins.rx_ws, pins.rx_dout);
    return false;
}

bool I2sAudioDevice::start() {
    if (capturing_) return false;  // half-duplex
    playing_ = true;
    return true;
}

bool I2sAudioDevice::stop() {
    playing_ = false;
    return true;
}

bool I2sAudioDevice::play(const std::vector<std::uint8_t>& pcm) {
    if (!playing_ || pcm.empty()) return false;
    std::size_t written = 0;
    // Real write via i2s_channel_write once handles exist (UNVERIFIED).
    (void)written;
    return true;
}

bool I2sAudioDevice::start_capture() {
    if (playing_ || capturing_) return false;  // half-duplex, no overlap
    capturing_ = true;
    return true;
}

std::vector<std::uint8_t> I2sAudioDevice::stop_capture() {
    if (!capturing_) return {};
    capturing_ = false;
    // Real read via i2s_channel_read loop, capped at max_capture_pcm_bytes().
    (void)kCaptureTimeoutMs;
    return {};
}

}  // namespace jalak::hal

#endif  // ESP_PLATFORM
