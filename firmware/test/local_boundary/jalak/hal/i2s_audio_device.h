#pragma once
// Host boundary stub for the I2S audio device (TX-only in the local demo).
// Records enable/play calls; the harness asserts the mic (RX) is never opened.
#include "boundary.h"
namespace jalak::hal {
struct I2sPins { int tx_bclk, tx_ws, tx_din, rx_bclk, rx_ws, rx_dout; };
class I2sAudioDevice {
public:
    bool begin(I2sPins pins) {
        if (pins.tx_bclk != 4 || pins.tx_ws != 5 || pins.tx_din != 6)
            throw std::runtime_error("i2s tx wiring");
        if (pins.rx_bclk >= 0 || pins.rx_ws >= 0 || pins.rx_dout >= 0)
            throw std::runtime_error("mic must stay disabled in local demo");
        ++boundary::i2s_begin;
        return boundary::i2s_ok;
    }
    bool start() {
        if (boundary::capturing) throw std::runtime_error("half-duplex violation");
        boundary::playing = true; return true;
    }
    bool stop() { boundary::playing = false; return true; }
    bool play(const std::vector<std::uint8_t>& pcm) {
        if (pcm.empty() || pcm.size() % 2 != 0) throw std::runtime_error("pcm");
        ++boundary::plays; return true;
    }
    bool is_playing() const { return boundary::playing; }
    bool start_capture() { ++boundary::mic; throw std::runtime_error("mic disabled"); return false; }
    std::vector<std::uint8_t> stop_capture() { throw std::runtime_error("mic disabled"); }
    bool is_capturing() const { return false; }
};
}
