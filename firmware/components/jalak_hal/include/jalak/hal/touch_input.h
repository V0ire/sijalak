#pragma once
// IDF-backed touch/PTT input driver.
//
// UNVERIFIED: requires a board + a known touch controller (e.g. CST816S) and its
// INT/RST pins, all unknown until G1. Compiles only under ESP-IDF.
//
// The driver only reports raw press state + a monotonic duration; classification
// into Tap/DoubleTap/LongPress uses the portable, host-tested
// jalak::classify_press (state_machine.h). Keeping that split means the gesture
// policy is tested on the host while the electrical part stays thin.

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

// A minimal debounced reader over a GPIO/IRQ line. The electrical specifics
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
