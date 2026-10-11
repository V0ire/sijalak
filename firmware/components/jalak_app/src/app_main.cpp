// Temporary LOCAL-ONLY demo: OLED + GPIO2 touch + optional SD animations/sound.
// No voice entrypoint, no network, no opt-in cloud path. Target/hardware UNVERIFIED.
#if defined(ESP_PLATFORM)
#include <string>
#include <vector>

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "jalak/local/clip.h"
#include "jalak/local/demo.h"
#include "jalak/local/sounds.h"
#include "jalak/hal/i2c_oled_display.h"
#include "jalak/hal/i2s_audio_device.h"
#include "jalak/hal/sd_card.h"
#include "jalak/hal/touch_input.h"

#ifndef CONFIG_JALAK_OLED_SH1106
#define CONFIG_JALAK_OLED_SH1106 0
#endif
#ifndef CONFIG_JALAK_TOUCH_ACTIVE_LOW
#define CONFIG_JALAK_TOUCH_ACTIVE_LOW 0
#endif

namespace {
constexpr int kI2cSda = 8, kI2cScl = 9;
constexpr int kTouch = 2;
// Owner-reported amp pins (UNVERIFIED). Mic RX is not used by the local demo.
constexpr int kTxBclk = 4, kTxWs = 5, kTxDin = 6;
// Owner-reported SD pins (UNVERIFIED; check the module datasheet before wiring).
constexpr int kSdCs = 10, kSdMosi = 11, kSdSck = 12, kSdMiso = 13;

// Turn a raw .pcm file (16-bit mono) into a Clip-less PCM buffer. Helper kept
// here so the portable logic stays free of file I/O.
void play_pcm(jalak::hal::I2sAudioDevice& audio, const std::vector<std::uint8_t>& pcm) {
    if (pcm.empty()) return;
    if (audio.start()) {
        audio.play(pcm);
        audio.stop();
    }
}
}  // namespace

extern "C" void app_main(void) {
    constexpr char tag[] = "jalak_local";
    jalak::local::Demo demo;
    jalak::local::Button button;
    jalak::Framebuffer fb({128, 64});

    jalak::hal::I2cOledDisplay display;
    jalak::hal::OledConfig config;
    config.controller = CONFIG_JALAK_OLED_SH1106 ? jalak::hal::OledController::Sh1106
                                               : jalak::hal::OledController::Ssd1306;
    config.column_offset = CONFIG_JALAK_OLED_SH1106 ? 2 : 0;
    config.i2c_address = CONFIG_JALAK_OLED_ADDRESS;
    bool display_ok = display.begin({kI2cSda, kI2cScl, 400000, 0}, config);
    if (!display_ok) ESP_LOGW(tag, "OLED unavailable; demo continues, no retries");

    jalak::hal::GpioTouchInput touch(kTouch, CONFIG_JALAK_TOUCH_ACTIVE_LOW);
    const bool touch_ok = touch.begin();
    if (!touch_ok) ESP_LOGW(tag, "GPIO2 unavailable; input disabled");

    jalak::hal::I2sAudioDevice audio;
    const bool audio_ok = audio.begin({kTxBclk, kTxWs, kTxDin, -1, -1, -1}, jalak::local::kAudioRateHz);
    if (!audio_ok) ESP_LOGW(tag, "I2S amp unavailable; running silently");

    jalak::hal::SdCard sd;
    const bool sd_ok = sd.begin({kSdCs, kSdMosi, kSdSck, kSdMiso});
    if (!sd_ok) {
        ESP_LOGW(tag, "SD unavailable; using built-in eyes, no sound");
    } else {
        // Load one animation for the Face screen (first available, best-effort).
        for (const char* name : {"happy", "love", "wink", "sleepy"}) {
            const std::string path = std::string("/sdcard/frames/") + name + ".bin";
            const auto blob = sd.read_file(path);
            if (blob.empty()) continue;
            const std::size_t frames = blob.size() / jalak::local::kFrameBytes;
            jalak::local::Clip clip;
            clip.name = name;
            clip.frame_delay_ms = 100;
            clip.frames = jalak::local::unpack_frames(blob, frames);
            if (demo.load_face_clip(std::move(clip))) {
                ESP_LOGI(tag, "loaded animation '%s' (%zu frames)", name, frames);
                break;
            }
        }
    }
    const auto pop = sd_ok ? sd.read_file("/sdcard/pop.pcm") : std::vector<std::uint8_t>{};

    ESP_LOGI(tag, "LOCAL ONLY: OLED + GPIO2 (+optional SD); controller/address/polarity/pins UNVERIFIED");
    for (;;) {
        const auto now = static_cast<std::uint64_t>(esp_timer_get_time() / 1000);
        demo.update(now, button.sample(touch_ok && touch.is_pressed(), now));

        if (audio_ok) {
            const auto req = demo.take_sound();
            if (req.play) {
                if (req.is_pop && !pop.empty()) {
                    play_pcm(audio, jalak::local::pitch_shift_pcm16(pop, 236 + (now % 41)));
                } else if (!req.is_pop) {
                    play_pcm(audio, jalak::local::render_tone(req.tone));
                }
            }
        } else {
            (void)demo.take_sound();
        }

        demo.render(fb);
        if (display_ok && !display.present(fb.to_page_bytes())) {
            display_ok = false;
            ESP_LOGW(tag, "OLED write failed; further writes disabled, timer continues");
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
#endif
