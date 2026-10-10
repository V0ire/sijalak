#pragma once
// IDF-backed display driver: I2C OLED implementing jalak::hal::Display.
//
// UNVERIFIED: requires an ESP32-S3 board, a real SSD1306/SH1106 module, and the
// correct I2C pins/address (all unknown until G1 evidence exists). It translates
// the portable OledPanel/OledTransaction sequences (jalak/oled_codec.h) into
// i2c_master writes. Host tests cannot exercise this file (it includes IDF).
//
// This header is safe to include from host code ONLY if the IDF guards below are
// not triggered; prefer including it from app_main.cpp, which is IDF-only.

#include <cstdint>
#include <vector>

#include "jalak/hal/display.h"
#include "jalak/oled_codec.h"

#if defined(ESP_PLATFORM)
#include "driver/i2c_master.h"
#include "esp_err.h"

namespace jalak::hal {

struct I2cOledPins {
    int sda = -1;
    int scl = -1;
    std::uint32_t clock_hz = 400000;
    int i2c_port = 0;
};

class I2cOledDisplay final : public Display {
public:
    bool init(const OledConfig& cfg) override;
    bool present(const std::vector<std::uint8_t>& page_bytes) override;
    bool is_ready() const override { return ready_; }

    // Configure the bus + panel. Call once before init(). Returns false on any
    // IDF error; the driver is then not ready and present() is a no-op.
    bool begin(const I2cOledPins& pins, const OledConfig& cfg);

private:
    bool write(const OledTransaction& txn);
    bool initWithPanel(const OledConfig& cfg);
    i2c_master_bus_handle_t bus_ = nullptr;
    i2c_master_dev_handle_t dev_ = nullptr;
    OledPanel panel_{};
    bool ready_ = false;
};

}  // namespace jalak::hal
#endif  // ESP_PLATFORM
