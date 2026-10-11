// IDF I2S audio driver — real implementation for ESP-IDF v5.4 (i2s_std API).
//
// Two independent channels: TX to the MAX98357A amp (24 kHz PCM16 mono) and RX
// from the INMP441 mic (16 kHz, 32-bit slots converted to PCM16 mono).
//
// Half-duplex is enforced in software (never capture while playing) AND the
// channels are only enabled when used.
//
// UNVERIFIED on hardware.

#if defined(ESP_PLATFORM)

#include "jalak/hal/i2s_audio_device.h"

#include <algorithm>
#include <cstring>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace jalak::hal {

namespace {
constexpr char kTag[] = "jalak_i2s";

// Canonical formats (must match the backend / jalak::audio limits).
constexpr uint32_t kPlaybackRate = 24000;  // output 24 kHz
constexpr uint32_t kCaptureRate = 16000;   // input 16 kHz
constexpr int kTxTimeoutMs = 200;
constexpr int kRxTimeoutMs = 200;

// Worst case capture size in bytes at 32-bit slots; bounded by the canonical cap.
std::size_t max_capture_pcm_bytes() {
    return jalak::pcm_bytes_for_ms(16000, 2, 8000);
}
}  // namespace

I2sAudioDevice::~I2sAudioDevice() {
    if (tx_ != nullptr) {
        i2s_channel_disable(tx_);
        i2s_del_channel(tx_);
        tx_ = nullptr;
    }
    if (rx_ != nullptr) {
        i2s_channel_disable(rx_);
        i2s_del_channel(rx_);
        rx_ = nullptr;
    }
}

bool I2sAudioDevice::begin(const I2sPins& pins) {
    if (pins.tx_bclk < 0 || pins.tx_ws < 0 || pins.tx_din < 0) {
        ESP_LOGE(kTag, "TX pins invalid (%d/%d/%d)", pins.tx_bclk, pins.tx_ws, pins.tx_din);
        return false;
    }
    if (pins.rx_bclk < 0 || pins.rx_ws < 0 || pins.rx_dout < 0) {
        ESP_LOGE(kTag, "RX pins invalid (%d/%d/%d)", pins.rx_bclk, pins.rx_ws, pins.rx_dout);
        return false;
    }

    // --- TX channel (full-duplex = false; we create two half-duplex channels) --
    i2s_chan_config_t tx_chan = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    if (i2s_new_channel(&tx_chan, &tx_, nullptr) != ESP_OK) {
        ESP_LOGE(kTag, "i2s_new_channel(TX) failed");
        return false;
    }
    i2s_std_config_t tx_cfg = {};
    tx_cfg.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(kPlaybackRate);
    tx_cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO);
    tx_cfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
    tx_cfg.gpio_cfg.bclk = (gpio_num_t)pins.tx_bclk;
    tx_cfg.gpio_cfg.ws = (gpio_num_t)pins.tx_ws;
    tx_cfg.gpio_cfg.dout = (gpio_num_t)pins.tx_din;
    tx_cfg.gpio_cfg.din = I2S_GPIO_UNUSED;
    if (i2s_channel_init_std_mode(tx_, &tx_cfg) != ESP_OK) {
        ESP_LOGE(kTag, "init_std_mode(TX) failed");
        return false;
    }

    // --- RX channel (mic; 32-bit slots, converted to PCM16 on read) -----------
    i2s_chan_config_t rx_chan = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    if (i2s_new_channel(&rx_chan, nullptr, &rx_) != ESP_OK) {
        ESP_LOGE(kTag, "i2s_new_channel(RX) failed");
        return false;
    }
    i2s_std_config_t rx_cfg = {};
    rx_cfg.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(kCaptureRate);
    rx_cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO);
    rx_cfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
    rx_cfg.gpio_cfg.bclk = (gpio_num_t)pins.rx_bclk;
    rx_cfg.gpio_cfg.ws = (gpio_num_t)pins.rx_ws;
    rx_cfg.gpio_cfg.dout = I2S_GPIO_UNUSED;
    rx_cfg.gpio_cfg.din = (gpio_num_t)pins.rx_dout;
    if (i2s_channel_init_std_mode(rx_, &rx_cfg) != ESP_OK) {
        ESP_LOGE(kTag, "init_std_mode(RX) failed");
        return false;
    }

    ready_ = true;
    ESP_LOGI(kTag, "I2S ready: TX(%d/%d/%d)@%uHz RX(%d/%d/%d)@%uHz",
             pins.tx_bclk, pins.tx_ws, pins.tx_din, (unsigned)kPlaybackRate,
             pins.rx_bclk, pins.rx_ws, pins.rx_dout, (unsigned)kCaptureRate);
    return true;
}

bool I2sAudioDevice::start() {
    if (!ready_ || capturing_) return false;  // half-duplex
    if (i2s_channel_enable(tx_) != ESP_OK) return false;
    playing_ = true;
    return true;
}

bool I2sAudioDevice::stop() {
    if (tx_ == nullptr) return false;
    i2s_channel_disable(tx_);
    playing_ = false;
    return true;
}

bool I2sAudioDevice::play(const std::vector<std::uint8_t>& pcm) {
    if (!playing_ || pcm.empty()) return false;
    const std::size_t aligned = pcm.size() - (pcm.size() % 2);  // PCM16 => even bytes
    std::size_t written = 0;
    const esp_err_t err =
        i2s_channel_write(tx_, pcm.data(), aligned, &written, kTxTimeoutMs);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "i2s_channel_write failed: %s", esp_err_to_name(err));
        return false;
    }
    return written == aligned;
}

bool I2sAudioDevice::start_capture() {
    if (!ready_ || playing_ || capturing_) return false;  // half-duplex
    if (i2s_channel_enable(rx_) != ESP_OK) return false;
    capturing_ = true;
    return true;
}

std::vector<std::uint8_t> I2sAudioDevice::stop_capture() {
    std::vector<std::uint8_t> out;
    if (rx_ == nullptr || !capturing_) return out;
    capturing_ = false;
    i2s_channel_disable(rx_);

    // Read 32-bit slots into a temp buffer up to the canonical cap, then convert
    // to PCM16 by taking the top 16 bits of each 32-bit word.
    const std::size_t pcm16_cap = max_capture_pcm_bytes();
    std::vector<std::int32_t> raw(pcm16_cap / 2);  // same sample count
    std::size_t bytes_read = 0;
    const esp_err_t err =
        i2s_channel_read(rx_, raw.data(), raw.size() * sizeof(std::int32_t),
                         &bytes_read, kRxTimeoutMs);
    if (err != ESP_OK && err != ESP_ERR_TIMEOUT) {
        ESP_LOGE(kTag, "i2s_channel_read failed: %s", esp_err_to_name(err));
        return out;
    }
    const std::size_t samples = bytes_read / sizeof(std::int32_t);
    out.resize(samples * 2);
    for (std::size_t i = 0; i < samples; ++i) {
        const std::int16_t s16 = (std::int16_t)(raw[i] >> 16);
        std::memcpy(&out[i * 2], &s16, sizeof(s16));
    }
    return out;
}

}  // namespace jalak::hal

#endif  // ESP_PLATFORM
