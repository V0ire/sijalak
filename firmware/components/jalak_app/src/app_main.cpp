// =====================================================================
// Jalak APP — ESP-IDF glue / entry point.
//
// Composes the portable, host-tested jalak_logic with the IDF-backed drivers in
// jalak_hal. Built ONLY by idf.py (ESP_PLATFORM). UNVERIFIED on hardware: pins,
// I2C address, I2S clocking and the endpoint hostname are not measured. A clean
// compile is NOT a hardware validation.
//
// Orchestration (single loop):
//   PTT press  -> StartCapture (Listening; mic captures)
//   PTT release/timeout -> StopCapture (Uploading)
//   Uploading  -> submit turn -> UploadComplete (Processing)
//   Processing -> TurnSucceeded (Speaking) -> play audio -> SpeechFinished (Ready)
//   any failure-> TurnFailed (Error); long press -> STOP (aborts in-flight)
//
// State transitions mirror jalak::StateMachine exactly (see state_machine.cpp).
// =====================================================================

#if defined(ESP_PLATFORM)

#include <string>
#include <vector>

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
// Board wiring — ESP32-S3 (N16R8).
// OWNER-REPORTED pins; UNVERIFIED against real hardware. OLED controller and
// I2C address are NOT confirmed.
//   I2C (OLED):   SDA=8, SCL=9
//   I2S amp (MAX98357A, TX): BCLK=4, LRC=5, DIN=6
//   I2S mic (INMP441, RX):   BCLK=18, WS=47, DOUT=21   (agent-chosen)
//   Touch/PTT:    IRQ=2
//   microSD (SPI, NOT driven): CS=10, MOSI=11, SCK=12, MISO=13
// WARNING: N16R8 octal PSRAM uses GPIO 11-17/33-37, so the SD SPI mapping above
// is LIKELY INVALID and must be re-checked on hardware before wiring SD.
// ---------------------------------------------------------------------------
constexpr int kI2cSda = 8;
constexpr int kI2cScl = 9;
constexpr int kTouchIrq = 2;
constexpr int kI2sTxBclk = 4;
constexpr int kI2sTxWs = 5;
constexpr int kI2sTxDin = 6;
constexpr int kI2sRxBclk = 18;
constexpr int kI2sRxWs = 47;
constexpr int kI2sRxDout = 21;
[[maybe_unused]] constexpr int kSdCs = 10;
[[maybe_unused]] constexpr int kSdMosi = 11;
[[maybe_unused]] constexpr int kSdSck = 12;
[[maybe_unused]] constexpr int kSdMiso = 13;

constexpr int kFrameIntervalMs = 33;  // ~30 FPS render loop
constexpr std::uint32_t kCaptureRateHz = 16000;

long long now_ms() { return esp_timer_get_time() / 1000; }

// Wrap raw PCM16 mono into a minimal 44-byte WAV header so the backend's
// canonical-WAV validation can read it.
std::vector<std::uint8_t> wrap_wav_pcm16(const std::vector<std::uint8_t>& pcm,
                                         std::uint32_t sample_rate) {
    std::vector<std::uint8_t> wav;
    wav.reserve(44 + pcm.size());
    auto put32 = [&](std::uint32_t v) {
        wav.push_back((std::uint8_t)(v & 0xFF));
        wav.push_back((std::uint8_t)((v >> 8) & 0xFF));
        wav.push_back((std::uint8_t)((v >> 16) & 0xFF));
        wav.push_back((std::uint8_t)((v >> 24) & 0xFF));
    };
    auto put16 = [&](std::uint16_t v) {
        wav.push_back((std::uint8_t)(v & 0xFF));
        wav.push_back((std::uint8_t)((v >> 8) & 0xFF));
    };
    const std::uint32_t data_len = (std::uint32_t)pcm.size();
    const std::uint16_t channels = 1;
    const std::uint16_t bits = 16;
    const std::uint32_t byte_rate = sample_rate * channels * (bits / 8);
    wav.insert(wav.end(), {'R', 'I', 'F', 'F'});
    put32(36 + data_len);
    wav.insert(wav.end(), {'W', 'A', 'V', 'E'});
    wav.insert(wav.end(), {'f', 'm', 't', ' '});
    put32(16);
    put16(1);  // PCM
    put16(channels);
    put32(sample_rate);
    put32(byte_rate);
    put16((std::uint16_t)(channels * (bits / 8)));
    put16(bits);
    wav.insert(wav.end(), {'d', 'a', 't', 'a'});
    put32(data_len);
    wav.insert(wav.end(), pcm.begin(), pcm.end());
    return wav;
}

// Run the upload->processing->speaking leg for one turn. Returns true if the
// reply was fetched and played (turn succeeded).
bool run_turn(jalak::DeviceApiClient& client, jalak::hal::I2sAudioDevice& audio,
              bool audio_ok, const std::string& session_id,
              const std::vector<std::uint8_t>& pcm, unsigned seq,
              jalak::TurnStatus* out_status) {
    using namespace jalak;
    const auto wav = wrap_wav_pcm16(pcm, kCaptureRateHz);
    TurnRequest req;
    req.session_id = session_id;
    req.idempotency_key = "jalak-" + std::to_string(seq);
    req.wav_bytes = wav;

    auto outcome = client.submit_turn(req);
    if (out_status) *out_status = outcome.ok ? outcome.response.status : TurnStatus::Failed;
    if (!outcome.ok) {
        ESP_LOGW(kTag, "submit failed: %s", outcome.error.reason.c_str());
        return false;
    }
    if (outcome.response.status != TurnStatus::Done) {
        return outcome.response.status == TurnStatus::Accepted ||
               outcome.response.status == TurnStatus::Processing;
    }

    // Fetch and play the reply audio (bounded by the client's download cap).
    auto audio_res = client.fetch_audio(outcome.response.turn_id, outcome.response.audio_bytes);
    if (audio_res.ok && audio_ok) {
        if (audio.start()) {
            audio.play(audio_res.wav_bytes);
            audio.stop();
        }
    }
    return true;
}

}  // namespace

extern "C" void app_main(void) {
    using namespace jalak;

    ESP_LOGI(kTag, "Si Jalak Bedas starting (UNVERIFIED build)");

    StateMachine sm;
    Framebuffer fb({128, 64});

    // --- Display ------------------------------------------------------------
    hal::I2cOledDisplay display;
    hal::I2cOledPins pins{kI2cSda, kI2cScl, 400000, 0};
    hal::OledConfig oled_cfg;  // controller/address MUST be confirmed on hardware
    oled_cfg.controller = hal::OledController::Ssd1306;
    const bool display_ok = display.begin(pins, oled_cfg) && display.init(oled_cfg);
    if (!display_ok) ESP_LOGW(kTag, "display init failed (pins/controller unverified)");

    // --- Touch / PTT --------------------------------------------------------
    hal::GpioTouchInput touch(kTouchIrq, /*active_low=*/true);
    if (!touch.begin()) ESP_LOGW(kTag, "touch init failed");

    // --- Audio (I2S) --------------------------------------------------------
    hal::I2sAudioDevice audio;
    const bool audio_ok = audio.begin({kI2sTxBclk, kI2sTxWs, kI2sTxDin,
                                       kI2sRxBclk, kI2sRxWs, kI2sRxDout});
    if (!audio_ok) ESP_LOGW(kTag, "audio init failed (pins unverified)");

    // --- Device API client --------------------------------------------------
    hal::HttpTransportConfig http_cfg;
    http_cfg.base_url = "https://UNKNOWN/v1";  // MUST be set from real evidence
    http_cfg.device_token = "dev-token";
    http_cfg.device_id = "jalak-01";
    hal::EspHttpTransport transport(http_cfg);
    CancellationToken cancel;
    DeviceApiClient client(transport);
    client.set_cancellation(&cancel);

    // Best-effort session bootstrap. If the host is unknown this fails
    // gracefully; the device stays idle rather than crashing.
    std::string session_id = transport.create_session();
    if (session_id.empty()) {
        ESP_LOGW(kTag, "no session (host unverified); device will idle");
    } else {
        ESP_LOGI(kTag, "session: %s", session_id.c_str());
    }

    // --- Boot ---------------------------------------------------------------
    sm.handle(Event::BootComplete);

    long long last_tick = now_ms();
    bool last_pressed = false;
    int press_start_ms = 0;
    unsigned turn_seq = 0;
    std::vector<std::uint8_t> captured_pcm;

    for (;;) {
        const long long t = now_ms();
        const int dt = static_cast<int>(t - last_tick);
        last_tick = t;

        // Capture timeout (8 s cap) is emitted as StopCapture by the FSM.
        (void)sm.tick(dt);

        // --- Input: raw press -> gesture -> events --------------------------
        const bool pressed = touch.is_pressed();
        if (pressed && !last_pressed) {
            press_start_ms = static_cast<int>(t);
        } else if (!pressed && last_pressed) {
            const Gesture g = classify_press(static_cast<int>(t) - press_start_ms);
            if (g == Gesture::Tap) {
                if (sm.state() == State::Error || sm.state() == State::Stopped) {
                    sm.handle(Event::Reset);
                }
                sm.handle(Event::StartCapture);
            } else if (g == Gesture::LongPress) {
                sm.handle(Event::Stop);      // explicit STOP
                cancel.request_abort();      // abort any in-flight request
            }
        }
        last_pressed = pressed;

        // --- Mic gating (half-duplex, driven by state) ----------------------
        if (sm.state() == State::Listening) {
            if (audio_ok && !audio.is_capturing()) audio.start_capture();
        } else if (audio.is_capturing()) {
            captured_pcm = audio.stop_capture();
        }

        // --- State-driven turn lifecycle ------------------------------------
        switch (sm.state()) {
            case State::Listening:
                // Release or timeout moves us out of Listening; handled above.
                break;
            case State::Uploading: {
                if (session_id.empty()) {
                    sm.handle(Event::TurnFailed);
                    break;
                }
                TurnStatus st = TurnStatus::Failed;
                const bool ok = run_turn(client, audio, audio_ok, session_id,
                                         captured_pcm, ++turn_seq, &st);
                captured_pcm.clear();
                if (ok) {
                    sm.handle(Event::UploadComplete);
                    sm.handle(Event::TurnSucceeded);
                    // Audio already played inside run_turn -> speech is finished.
                    sm.handle(Event::SpeechFinished);
                } else {
                    sm.handle(Event::TurnFailed);
                }
                cancel.reset();
                break;
            }
            default:
                break;
        }

        // --- Render ---------------------------------------------------------
        draw_face(fb, sm.current_expression());
        if (display_ok) display.present(fb.to_page_bytes());

        vTaskDelay(pdMS_TO_TICKS(kFrameIntervalMs));
    }
}

#endif  // ESP_PLATFORM
