#pragma once
// IDF-backed I2S audio driver: RX (INMP441-style mic) + TX (MAX98357A amp).
//
// Real ESP-IDF v5.4 implementation using the modern i2s_std API:
//   i2s_new_channel -> i2s_channel_init_std_mode -> i2s_channel_enable
//   i2s_channel_write (TX) / i2s_channel_read (RX)
//
// UNVERIFIED on hardware: pins/clocking are owner/agent-reported, not measured.
// A successful compile is NOT a hardware validation.
//
// Format contract (must match the backend): input PCM16 mono @16 kHz, output
// PCM16 mono @24 kHz by default (local demo selects 16 kHz). Optional INMP441 RX
// emits 24/32-bit slots; the read path converts its 32-bit slots to PCM16.

#include <cstdint>
#include <vector>

#include "jalak/audio.h"

#if defined(ESP_PLATFORM)

#include "driver/i2s_std.h"

namespace jalak::hal {

// Separate TX (amp) and RX (mic) pin groups. On the Jalak board these are two
// distinct I2S controllers because they use different BCLK/WS lines. If a future
// revision shares BCLK/WS, set the same numbers in both groups.
struct I2sPins {
    // TX -> MAX98357A amplifier (speaker out).
    int tx_bclk = -1;
    int tx_ws = -1;
    int tx_din = -1;   // data OUT to the amp
    // RX <- INMP441 microphone.
    int rx_bclk = -1;
    int rx_ws = -1;
    int rx_dout = -1;  // data IN from the mic
};

class I2sAudioDevice final : public AudioSource, public AudioSink {
public:
    ~I2sAudioDevice() override;

    I2sAudioDevice() = default;
    I2sAudioDevice(const I2sAudioDevice&) = delete;
    I2sAudioDevice& operator=(const I2sAudioDevice&) = delete;
    // All RX pins -1 disables the microphone; partial RX configuration is invalid.
    bool begin(const I2sPins& pins, std::uint32_t playback_rate = 24000);

    // AudioSink (playback, configured rate mono PCM16).
    bool start() override;
    bool stop() override;
    bool play(const std::vector<std::uint8_t>& pcm) override;
    bool is_playing() const override { return playing_; }

    // AudioSource (capture, 16 kHz mono PCM16, capped at the canonical input cap).
    bool start_capture() override;
    std::vector<std::uint8_t> stop_capture() override;
    bool is_capturing() const override { return capturing_; }

private:
    void release_channels();
    i2s_chan_handle_t tx_ = nullptr;
    i2s_chan_handle_t rx_ = nullptr;
    bool ready_ = false;
    bool playing_ = false;
    bool capturing_ = false;
};

}  // namespace jalak::hal
#endif  // ESP_PLATFORM
