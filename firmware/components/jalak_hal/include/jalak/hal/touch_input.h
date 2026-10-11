#pragma once
// IDF-backed touch/PTT input driver.
//
// UNVERIFIED: GPIO digital-output capacitive module or button, not a native
// capacitive-pad scanner or I2C touch controller. Reports raw level only.
// Debounce/gestures belong to jalak::local::Button; clock supplied there.

#include <cstdint>

#if defined(ESP_PLATFORM)

namespace jalak::hal {

class TouchInput {
public:
    // Returns true if currently pressed. `now_ms` is a monotonic clock the
    // caller supplies (e.g. esp_timer_get_time()/1000).
    virtual ~TouchInput() = default;
    virtual bool is_pressed() = 0;
};

// A minimal raw reader over a GPIO line. The electrical specifics
// (active level, pull-up, controller protocol) must be confirmed on hardware.
class GpioTouchInput final : public TouchInput {
public:
    GpioTouchInput(int irq_gpio, bool active_low = true)
        : irq_gpio_(irq_gpio), active_low_(active_low) {}
    bool begin();
    bool is_pressed() override;

private:
    int irq_gpio_;
    bool active_low_;
};

}  // namespace jalak::hal
#endif  // ESP_PLATFORM
