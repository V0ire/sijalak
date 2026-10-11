#pragma once
#include <cstdint>
#include <stdexcept>
#include "jalak/hal/display.h"
namespace boundary {
inline int display_begin = 0, writes = 0, touch_begin = 0, polls = 0, delays = 0;
inline int i2s = 0, wifi = 0, http = 0, mic = 0;
inline int sd_begin = 0, i2s_begin = 0, plays = 0;
inline bool begin_ok = true, write_ok = true, touch_ok = true;
inline bool sd_ok = true, i2s_ok = true, playing = false, capturing = false;
inline std::int64_t clock_us = 0;
template<class... T> void log(T...) {}
inline void reset() { display_begin = writes = touch_begin = polls = delays = 0;
    i2s = wifi = http = mic = 0;
    sd_begin = i2s_begin = plays = 0; sd_ok = i2s_ok = true; playing = capturing = false;
    clock_us = 0; }
struct Finished {};
}
namespace jalak::hal {
struct I2cOledPins { int sda, scl; std::uint32_t clock_hz; int i2c_port; };
class I2cOledDisplay {
public:
    bool begin(I2cOledPins p, OledConfig c) {
        if (p.sda != 8 || p.scl != 9 || c.i2c_address != 0x3c) throw std::runtime_error("wiring");
#ifdef TEST_ACTIVE_HIGH_SH1106
        if (c.controller != OledController::Sh1106 || c.column_offset != 2) throw std::runtime_error("controller");
#else
        if (c.controller != OledController::Ssd1306 || c.column_offset != 0) throw std::runtime_error("controller");
#endif
        ++boundary::display_begin; return boundary::begin_ok;
    }
    bool present(const std::vector<std::uint8_t>& bytes) {
        if (bytes.size() != 1024) throw std::runtime_error("frame");
        ++boundary::writes; return boundary::write_ok;
    }
};
class GpioTouchInput {
public:
    GpioTouchInput(int pin, bool active_low) {
#ifdef TEST_ACTIVE_HIGH_SH1106
        if (pin != 2 || active_low) throw std::runtime_error("touch wiring");
#else
        if (pin != 2 || !active_low) throw std::runtime_error("touch wiring");
#endif
    }
    bool begin() { ++boundary::touch_begin; return boundary::touch_ok; }
    bool is_pressed() { ++boundary::polls; return false; }
};
}
inline std::int64_t esp_timer_get_time() { return boundary::clock_us; }
inline void vTaskDelay(int ticks) {
    if (ticks != 20) throw std::runtime_error("tick");
    boundary::clock_us += 20000;
    if (++boundary::delays == 10) throw boundary::Finished{};
}
#define pdMS_TO_TICKS(x) (x)
#define ESP_LOGW(...) boundary::log(__VA_ARGS__)
#define ESP_LOGI(...) boundary::log(__VA_ARGS__)
