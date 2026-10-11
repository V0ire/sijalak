#pragma once
// IDF-backed microSD (SPI + FATFS) for the local demo.
//
// UNVERIFIED on hardware: pins are owner-reported, the module's wiring/pull-ups
// are unconfirmed, and the S3 flash/PSRAM pin groups must be checked against the
// actual module datasheet before soldering. A clean compile is NOT a hardware
// validation.
//
// Owner-reported pins:
//   CS=10, MOSI=11, SCK=12, MISO=13
// (Earlier notes claiming GPIO 11-13 are flash/PSRAM on N16R8 were WRONG; on
// S3 the SPI0/1 flash group is GPIO26-32, and some OPI modules use 33-37. Keep
// the owner wiring and verify from the datasheet.)

#include <cstdint>
#include <string>
#include <vector>

#if defined(ESP_PLATFORM)

namespace jalak::hal {

struct SdPins {
    int cs = -1;
    int mosi = -1;
    int sck = -1;
    int miso = -1;
    int spi_host = 2;  // SPI2_HOST by default
};

class SdCard {
public:
    // Mount FATFS at /sdcard. Returns false on any failure (no card, bad FS).
    bool begin(const SdPins& pins, const char* mount_point = "/sdcard");
    bool mounted() const { return mounted_; }
    void end();

    // Read a whole file into memory. Returns empty on error. Bounded by max_bytes
    // so a huge/loop file can never exhaust RAM.
    std::vector<std::uint8_t> read_file(const std::string& path,
                                        std::size_t max_bytes = 2u * 1024u * 1024u);
    bool exists(const std::string& path) const;

private:
    bool mounted_ = false;
    char mount_[32] = {};
};

}  // namespace jalak::hal
#endif  // ESP_PLATFORM
