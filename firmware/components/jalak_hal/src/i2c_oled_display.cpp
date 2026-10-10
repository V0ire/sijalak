// IDF I2C OLED driver implementation. Compiles only under ESP-IDF (ESP_PLATFORM).
// UNVERIFIED: no board verified this. Logic delegated to the portable, host-tested
// jalak::oled_codec sequences.

#if defined(ESP_PLATFORM)

#include "jalak/hal/i2c_oled_display.h"

#include <cstring>

#include "esp_log.h"

namespace jalak::hal {

namespace {
constexpr char kTag[] = "jalak_oled";
constexpr int kI2cTimeoutMs = 50;
}  // namespace

bool I2cOledDisplay::begin(const I2cOledPins& pins, const OledConfig& cfg) {
    if (pins.sda < 0 || pins.scl < 0) {
        ESP_LOGE(kTag, "invalid I2C pins (sda=%d scl=%d)", pins.sda, pins.scl);
        return false;
    }
    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = pins.i2c_port;
    bus_cfg.sda_io_num = pins.sda;
    bus_cfg.scl_io_num = pins.scl;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.glitch_ignore_cnt = 7;
    bus_cfg.flags.enable_internal_pullup = true;
    if (i2c_new_master_bus(&bus_cfg, &bus_) != ESP_OK) {
        ESP_LOGE(kTag, "i2c_new_master_bus failed");
        return false;
    }
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = cfg.i2c_address;
    dev_cfg.scl_speed_hz = pins.clock_hz;
    if (i2c_master_bus_add_device(bus_, &dev_cfg, &dev_) != ESP_OK) {
        ESP_LOGE(kTag, "i2c_master_bus_add_device failed");
        return false;
    }
    return initWithPanel(cfg);
}

bool I2cOledDisplay::initWithPanel(const OledConfig& cfg) {
    panel_.width = 128;
    panel_.height = 64;
    panel_.controller = (cfg.controller == OledController::Sh1106)
                            ? OledControllerKind::Sh1106
                            : OledControllerKind::Ssd1306;
    panel_.column_offset = cfg.column_offset;
    panel_.rotate_180 = cfg.rotate_180;

    ready_ = true;  // allow write() during init
    for (const auto& txn : oled_init_sequence(panel_)) {
        if (!write(txn)) {
            ready_ = false;
            return false;
        }
    }
    return true;
}

bool I2cOledDisplay::init(const OledConfig& cfg) {
    // init() is the Display-interface entry; begin() must have set up the bus.
    if (dev_ == nullptr) {
        ESP_LOGE(kTag, "begin() must be called before init()");
        return false;
    }
    return initWithPanel(cfg);
}

bool I2cOledDisplay::present(const std::vector<std::uint8_t>& page_bytes) {
    if (!ready_ || !oled_frame_size_ok(panel_, page_bytes.size())) return false;
    for (const auto& txn : oled_full_frame(panel_, page_bytes)) {
        if (!write(txn)) return false;
    }
    return true;
}

bool I2cOledDisplay::write(const OledTransaction& txn) {
    if (dev_ == nullptr) return false;
    // The control byte is the first byte of the I2C payload.
    std::vector<std::uint8_t> buf;
    buf.reserve(txn.payload.size() + 1);
    buf.push_back(txn.control);
    buf.insert(buf.end(), txn.payload.begin(), txn.payload.end());
    return i2c_master_transmit(dev_, buf.data(), buf.size(), kI2cTimeoutMs) == ESP_OK;
}

}  // namespace jalak::hal

#endif  // ESP_PLATFORM
