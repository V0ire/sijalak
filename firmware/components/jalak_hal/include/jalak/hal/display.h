#pragma once
// Hardware abstraction for the display. The owned OLED's controller is unknown
// (SH1106 vs SSD1306), so the driver is selectable and defaults to a safe
// no-op used by host tests. Real I2C/SPI wiring is UNVERIFIED.

#include <cstdint>
#include <vector>

#include "jalak/framebuffer.h"

namespace jalak::hal {

enum class OledController {
    Sh1106,   // 132x64 internal RAM, common "1.3 inch" modules
    Ssd1306,  // 128x64
    Mock,     // no hardware; records pushes for tests
};

struct OledConfig {
    OledController controller = OledController::Mock;
    int i2c_address = 0x3C;      // typical; MUST be confirmed on real hardware
    bool rotate_180 = false;
    int column_offset = 0;       // SH1106 typically needs a small offset
};

class Display {
public:
    virtual ~Display() = default;
    virtual bool init(const OledConfig& cfg) = 0;
    // Push an entire 1-bit page-format framebuffer. Implementations translate
    // to controller commands; this interface is controller-agnostic.
    virtual bool present(const std::vector<std::uint8_t>& page_bytes) = 0;
    virtual bool is_ready() const = 0;
};

}  // namespace jalak::hal
