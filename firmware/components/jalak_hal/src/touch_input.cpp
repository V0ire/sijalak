// IDF touch input implementation. Compiles only under ESP-IDF. UNVERIFIED.

#if defined(ESP_PLATFORM)

#include "jalak/hal/touch_input.h"

#include "driver/gpio.h"
#include "esp_log.h"

namespace jalak::hal {

namespace {
constexpr char kTag[] = "jalak_touch";
}  // namespace

bool GpioTouchInput::begin() {
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = (1ULL << irq_gpio_);
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = active_low_ ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = active_low_ ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE;
    cfg.intr_type = GPIO_INTR_DISABLE;  // polled here; IRQ optional later
    const esp_err_t err = gpio_config(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "gpio_config(%d) failed: %d", irq_gpio_, (int)err);
        return false;
    }
    return true;
}

bool GpioTouchInput::is_pressed() {
    const int level = gpio_get_level((gpio_num_t)irq_gpio_);
    return active_low_ ? (level == 0) : (level == 1);
}

}  // namespace jalak::hal

#endif  // ESP_PLATFORM
