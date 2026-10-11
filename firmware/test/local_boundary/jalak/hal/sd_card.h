#pragma once
// Host boundary stub for the SD card. The local demo MAY mount SD; the harness
// only records it. It never touches a filesystem.
#include "boundary.h"
namespace jalak::hal {
struct SdPins { int cs, mosi, sck, miso; int spi_host; };
class SdCard {
public:
    bool begin(SdPins pins, const char* = "/sdcard") {
        if (pins.cs != 10 || pins.mosi != 11 || pins.sck != 12 || pins.miso != 13)
            throw std::runtime_error("sd wiring");
        ++boundary::sd_begin;
        return boundary::sd_ok;
    }
    bool mounted() const { return boundary::sd_ok; }
    void end() {}
    std::vector<std::uint8_t> read_file(const std::string&, std::size_t = 0) { return {}; }
    bool exists(const std::string&) const { return false; }
};
}
