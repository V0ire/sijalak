#pragma once
// IDF-backed I2S audio driver: RX (INMP441-style mic) + TX (MAX98357A amp).
//
// UNVERIFIED: requires a board + known I2S pins/clocking. Compiles only under
// ESP-IDF. Enforces the portable half-duplex rule from jalak_hal/jalak_logic.
//
// This is the piece the capture/playback caps assume: canonical input is
// 16 kHz PCM16 mono; output is 24 kHz PCM16 mono.

#include <cstdint>
#include <vector>

#include "jalak/audio.h"

#if defined(ESP_PLATFORM)

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
    bool begin(const I2sPins& pins);

    // AudioSink (playback, 24 kHz).
    bool start() override;
    bool stop() override;
    bool play(const std::vector<std::uint8_t>& pcm) override;
    bool is_playing() const override { return playing_; }

    // AudioSource (capture, 16 kHz, capped at the canonical input cap).
    bool start_capture() override;
    std::vector<std::uint8_t> stop_capture() override;
    bool is_capturing() const override { return capturing_; }

private:
    bool playing_ = false;
    bool capturing_ = false;
};

}  // namespace jalak::hal
#endif  // ESP_PLATFORM
