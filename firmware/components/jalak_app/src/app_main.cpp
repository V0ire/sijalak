// =====================================================================
// Jalak APP — ESP-IDF glue / entry point.
//
// Composes the portable, host-tested jalak_logic with the IDF-backed drivers in
// jalak_hal. This file is built ONLY by idf.py (ESP_PLATFORM); it is not part of
// the host test build. It is UNVERIFIED against real hardware: pins, I2C address,
// I2S clocking, the endpoint hostname, and the touch controller are all unknown
// until G1 evidence exists. Do not treat a successful compile as hardware
// validation.
//
// The portable logic it drives is fully unit-tested on the host (ctest).
// =====================================================================

#if defined(ESP_PLATFORM)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "jalak/api_client.h"
#include "jalak/audio.h"
#include "jalak/framebuffer.h"
#include "jalak/oled_codec.h"
#include "jalak/state_machine.h"

#include "jalak/hal/display.h"
#include "jalak/hal/esp_http_transport.h"
#include "jalak/hal/i2c_oled_display.h"
#include "jalak/hal/i2s_audio_device.h"
#include "jalak/hal/touch_input.h"

namespace {

constexpr char kTag[] = "jalak_app";

// ---------------------------------------------------------------------------
// Board wiring — ESP32-S3.
//
// These values come from the OWNER-REPORTED pinout. They are still UNVERIFIED
// against a real board (no hardware test has run) and the OLED controller and
// I2C address remain unconfirmed. Do NOT treat this as hardware validation.
//
//   I2C (OLED):   SDA=8, SCL=9
//   I2S (amp MAX98357A, TX): BCLK=4, LRC=5, DIN=6
//   Touch/PTT:    IRQ=2
//   microSD (SPI, NOT yet driven): CS=10, MOSI=11, SCK=12, MISO=13
//
// NOT yet mapped (leave invalid until known):
//   - INMP441 microphone I2S DOUT   (kI2sDout; mic data line unknown)
// ---------------------------------------------------------------------------
constexpr int kI2cSda = 8;
constexpr int kI2cScl = 9;

// Touch/PTT IRQ line (owner-reported).
constexpr int kTouchIrq = 2;

// I2S TX to the MAX98357A amplifier.
constexpr int kI2sBclk = 4;
constexpr int kI2sWs = 5;    // LRC
constexpr int kI2sDin = 6;   // DIN -> amp
// INMP441 (mic) data output line is NOT yet known; -1 keeps RX disabled.
constexpr int kI2sDout = -1;

// microSD (SPI) — recorded for reference only; no SD driver is wired yet.
[[maybe_unused]] constexpr int kSdCs = 10;
[[maybe_unused]] constexpr int kSdMosi = 11;
[[maybe_unused]] constexpr int kSdSck = 12;
[[maybe_unused]] constexpr int kSdMiso = 13;

long long now_ms() { return esp_timer_get_time() / 1000; }

}  // namespace

extern "C" void app_main(void) {
    using namespace jalak;

    ESP_LOGI(kTag, "Si Jalak Bedas starting (UNVERIFIED build)");

    // --- Portable state machine + render buffer ----------------------------
    StateMachine sm;

    // --- Display ------------------------------------------------------------
    hal::I2cOledDisplay display;
    hal::I2cOledPins pins{kI2cSda, kI2cScl, 400000, 0};
    hal::OledConfig oled_cfg;  // controller/address MUST be confirmed on hardware
    oled_cfg.controller = hal::OledController::Ssd1306;
    bool display_ok = display.begin(pins, oled_cfg) && display.init(oled_cfg);
    ESP_LOGW(kTag, "display init %s (pins are placeholders)", display_ok ? "ok" : "FAILED");

    // --- Touch / PTT --------------------------------------------------------
    hal::GpioTouchInput touch(kTouchIrq, /*active_low=*/true);
    if (!touch.begin()) ESP_LOGW(kTag, "touch init failed");

    // --- Audio (I2S) --------------------------------------------------------
    hal::I2sAudioDevice audio;
    if (!audio.begin({kI2sBclk, kI2sWs, kI2sDin, kI2sDout}))
        ESP_LOGW(kTag, "audio init failed (stub until pins known)");

    // --- Device API client --------------------------------------------------
    hal::HttpTransportConfig http_cfg;
    http_cfg.base_url = "https://UNKNOWN/v1";  // MUST be set from real evidence
    http_cfg.device_token = "dev-token";
    http_cfg.device_id = "jalak-01";
    hal::EspHttpTransport transport(http_cfg);
    CancellationToken cancel;
    DeviceApiClient client(transport);
    client.set_cancellation(&cancel);

    // --- Boot ---------------------------------------------------------------
    sm.handle(Event::BootComplete);
    Framebuffer fb({128, 64});

    int64_t last_tick = now_ms();
    bool last_pressed = false;
    int press_start_ms = 0;

    for (;;) {
        const int64_t t = now_ms();
        const int dt = static_cast<int>(t - last_tick);
        last_tick = t;

        // Advance timers (capture cap, session cap).
        if (auto emitted = sm.tick(dt)) {
            ESP_LOGI(kTag, "tick emitted event %d", static_cast<int>(*emitted));
        }

        // Input: raw press -> gesture -> state machine events.
        const bool pressed = touch.is_pressed();
        if (pressed && !last_pressed) {
            press_start_ms = static_cast<int>(t);
        } else if (!pressed && last_pressed) {
            const Gesture g = classify_press(static_cast<int>(t) - press_start_ms);
            switch (g) {
                case Gesture::Tap:
                    sm.handle(Event::StartCapture);
                    break;
                case Gesture::LongPress:
                    sm.handle(Event::Stop);  // explicit STOP
                    cancel.request_abort();
                    break;
                case Gesture::DoubleTap:
                case Gesture::None:
                default:
                    break;
            }
        }
        last_pressed = pressed;

        // Render current expression and push it if the display is present.
        draw_face(fb, sm.current_expression());
        if (display_ok) display.present(fb.to_page_bytes());

        // Half-duplex mic gating is driven by the state machine.
        if (sm.state() == State::Listening && !audio.is_capturing()) {
            audio.start_capture();
        } else if (sm.state() != State::Listening && audio.is_capturing()) {
            // The capture buffer would be uploaded here via the API client once
            // the endpoint is known; audio.stop_capture() returns the PCM.
            (void)audio.stop_capture();
        }

        vTaskDelay(pdMS_TO_TICKS(33));  // ~30 FPS render / poll loop
    }
}

#endif  // ESP_PLATFORM
