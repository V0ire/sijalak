#pragma once
// Behavioral boundary doubles, NOT SDK compatibility or hardware validation.
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
using esp_err_t = int;
constexpr int ESP_OK = 0, ESP_FAIL = -1, ESP_ERR_TIMEOUT = -2;
inline const char* esp_err_to_name(int) { return "mock"; }
#define ESP_LOGE(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
#define ESP_LOGI(...) ((void)0)
enum gpio_num_t { GPIO_NUM_NC = -1, GPIO_NUM_0 = 0 };
enum spi_host_device_t { SPI1_HOST = 0, SPI2_HOST = 1, SPI3_HOST = 2 };
struct sdmmc_card_t {};
struct spi_bus_config_t { int mosi_io_num, miso_io_num, sclk_io_num, quadwp_io_num, quadhd_io_num, max_transfer_sz; };
struct sdmmc_host_t { int slot; };
struct sdspi_device_config_t { gpio_num_t gpio_cs; spi_host_device_t host_id; };
struct esp_vfs_fat_sdmmc_mount_config_t { bool format_if_mount_failed; int max_files; std::size_t allocation_unit_size; };
#define SDSPI_HOST_DEFAULT() sdmmc_host_t{SPI2_HOST}
#define SDSPI_DEVICE_CONFIG_DEFAULT() sdspi_device_config_t{GPIO_NUM_NC, SPI2_HOST}
constexpr int SDSPI_DEFAULT_DMA = 3;
struct MockChannel { bool enabled = false; bool rx = false; };
using i2s_chan_handle_t = MockChannel*;
struct i2s_chan_config_t { int id, role; };
struct i2s_std_clk_config_t { std::uint32_t sample_rate_hz; };
struct i2s_std_slot_config_t { int bits, mode; };
struct i2s_std_config_t {
    i2s_std_clk_config_t clk_cfg;
    i2s_std_slot_config_t slot_cfg;
    struct { gpio_num_t mclk, bclk, ws, dout, din; } gpio_cfg;
};
constexpr int I2S_NUM_AUTO = -1, I2S_ROLE_MASTER = 0, I2S_DATA_BIT_WIDTH_16BIT = 16,
              I2S_DATA_BIT_WIDTH_32BIT = 32, I2S_SLOT_MODE_MONO = 1;
constexpr gpio_num_t I2S_GPIO_UNUSED = GPIO_NUM_NC;
#define I2S_CHANNEL_DEFAULT_CONFIG(id, role) i2s_chan_config_t{id, role}
#define I2S_STD_CLK_DEFAULT_CONFIG(rate) i2s_std_clk_config_t{rate}
#define I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits, mode) i2s_std_slot_config_t{bits, mode}
enum i2c_port_t { I2C_NUM_0, I2C_NUM_1 };
using i2c_master_bus_handle_t = void*;
using i2c_master_dev_handle_t = void*;
struct i2c_master_bus_config_t {
    i2c_port_t i2c_port; gpio_num_t sda_io_num, scl_io_num;
    int clk_source, glitch_ignore_cnt;
    struct { bool enable_internal_pullup; } flags;
};
struct i2c_device_config_t { int dev_addr_length, device_address; std::uint32_t scl_speed_hz; };
constexpr int I2C_CLK_SRC_DEFAULT = 0, I2C_ADDR_BIT_LEN_7 = 7;
namespace idf_mock {
inline int live = 0, new_calls = 0, init_calls = 0, rx_calls = 0;
inline int fail_new_at = 0, fail_init_at = 0;
inline bool fail_write = false, short_write = false, fail_disable = false;
inline std::uint32_t tx_rate = 0;
inline bool bus_owned = false, mount_fail = false, unmount_fail = false, fail_bus_init = false;
inline int bus_init_calls = 0, unmount_calls = 0, free_calls = 0;
inline spi_host_device_t host_seen = SPI1_HOST;
inline sdmmc_card_t card;
inline std::string mounted_path;
inline void reset() {
    assert(live == 0 && !bus_owned);
    new_calls = init_calls = rx_calls = fail_new_at = fail_init_at = 0;
    fail_write = short_write = fail_disable = false; tx_rate = 0;
    mount_fail = unmount_fail = fail_bus_init = false; bus_init_calls = unmount_calls = free_calls = 0;
    mounted_path.clear(); host_seen = SPI1_HOST;
}
}
inline int i2s_new_channel(const i2s_chan_config_t*, i2s_chan_handle_t* tx, i2s_chan_handle_t* rx) {
    using namespace idf_mock;
    ++new_calls; if (new_calls == fail_new_at) return ESP_FAIL;
    auto handle = new MockChannel; handle->rx = rx != nullptr;
    if (rx) { *rx = handle; ++rx_calls; } else { *tx = handle; }
    ++live; return ESP_OK;
}
inline int i2s_channel_init_std_mode(i2s_chan_handle_t h, const i2s_std_config_t* cfg) {
    using namespace idf_mock;
    ++init_calls; if (!h->rx) tx_rate = cfg->clk_cfg.sample_rate_hz;
    return init_calls == fail_init_at ? ESP_FAIL : ESP_OK;
}
inline int i2s_channel_enable(i2s_chan_handle_t h) {
    if (!h || h->enabled) return ESP_FAIL;
    h->enabled = true; return ESP_OK;
}
inline int i2s_channel_disable(i2s_chan_handle_t h) {
    if (!h || !h->enabled || idf_mock::fail_disable) return ESP_FAIL;
    h->enabled = false; return ESP_OK;
}
inline int i2s_del_channel(i2s_chan_handle_t h) {
    assert(h && !h->enabled); delete h; --idf_mock::live; return ESP_OK;
}
inline int i2s_channel_write(i2s_chan_handle_t h, const void*, std::size_t n, std::size_t* written, int) {
    assert(h && h->enabled && !h->rx);
    *written = idf_mock::short_write ? n / 2 : n;
    return idf_mock::fail_write ? ESP_FAIL : ESP_OK;
}
inline int i2s_channel_read(i2s_chan_handle_t h, void*, std::size_t, std::size_t* n, int) {
    assert(h && h->enabled && h->rx); *n = 0; return ESP_OK;
}
inline int spi_bus_initialize(spi_host_device_t h, const spi_bus_config_t*, int) {
    assert(!idf_mock::bus_owned); ++idf_mock::bus_init_calls;
    if (idf_mock::fail_bus_init) return ESP_FAIL;
    idf_mock::bus_owned = true; idf_mock::host_seen = h; return ESP_OK;
}
inline int spi_bus_free(spi_host_device_t h) {
    assert(idf_mock::bus_owned && h == idf_mock::host_seen);
    idf_mock::bus_owned = false; ++idf_mock::free_calls; return ESP_OK;
}
inline int esp_vfs_fat_sdspi_mount(const char* path, const sdmmc_host_t* host,
                                  const sdspi_device_config_t* slot,
                                  const esp_vfs_fat_sdmmc_mount_config_t* cfg, sdmmc_card_t** card) {
    assert(host->slot == slot->host_id && !cfg->format_if_mount_failed);
    if (idf_mock::mount_fail) return ESP_FAIL;
    idf_mock::mounted_path = path; *card = &idf_mock::card; return ESP_OK;
}
inline int esp_vfs_fat_sdcard_unmount(const char* path, sdmmc_card_t* card) {
    assert(card == &idf_mock::card && path == idf_mock::mounted_path);
    ++idf_mock::unmount_calls; return idf_mock::unmount_fail ? ESP_FAIL : ESP_OK;
}
inline int i2c_new_master_bus(const i2c_master_bus_config_t*, i2c_master_bus_handle_t* h) { *h = &idf_mock::card; return ESP_OK; }
inline int i2c_master_bus_add_device(i2c_master_bus_handle_t, const i2c_device_config_t*, i2c_master_dev_handle_t* h) { *h = &idf_mock::card; return ESP_OK; }
inline int i2c_master_transmit(i2c_master_dev_handle_t, const void*, std::size_t, int) { return ESP_OK; }
