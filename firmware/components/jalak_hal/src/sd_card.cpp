// IDF microSD (SPI + FATFS) implementation. Compiles only under ESP-IDF.
// UNVERIFIED on hardware; see sd_card.h for the pin caveat.

#if defined(ESP_PLATFORM)

#include "jalak/hal/sd_card.h"

#include <cstdio>
#include <cstring>

#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

namespace jalak::hal {

namespace {
constexpr char kTag[] = "jalak_sd";
}  // namespace

bool SdCard::begin(const SdPins& pins, const char* mount_point) {
    if (mounted_) return true;
    if (!mount_point || mount_point[0] != '/' || std::strlen(mount_point) >= sizeof(mount_) ||
        (pins.spi_host != SPI2_HOST && pins.spi_host != SPI3_HOST)) return false;
    if (pins.cs < 0 || pins.mosi < 0 || pins.sck < 0 || pins.miso < 0) {
        ESP_LOGE(kTag, "invalid SD pins cs=%d mosi=%d sck=%d miso=%d",
                 pins.cs, pins.mosi, pins.sck, pins.miso);
        return false;
    }

    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {};
    mount_cfg.format_if_mount_failed = false;
    mount_cfg.max_files = 5;
    mount_cfg.allocation_unit_size = 16 * 1024;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = pins.spi_host;

    spi_bus_config_t bus = {};
    bus.mosi_io_num = pins.mosi;
    bus.miso_io_num = pins.miso;
    bus.sclk_io_num = pins.sck;
    bus.quadwp_io_num = -1;
    bus.quadhd_io_num = -1;
    bus.max_transfer_sz = 4000;

    if (spi_bus_initialize(pins.spi_host, &bus, SDSPI_DEFAULT_DMA) != ESP_OK) {
        ESP_LOGE(kTag, "spi_bus_initialize failed");
        return false;
    }

    sdspi_device_config_t slot = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot.gpio_cs = (gpio_num_t)pins.cs;
    slot.host_id = pins.spi_host;

    sdmmc_card_t* card = nullptr;
    const esp_err_t err =
        esp_vfs_fat_sdspi_mount(mount_point, &host, &slot, &mount_cfg, &card);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "mount failed: %s", esp_err_to_name(err));
        spi_bus_free(pins.spi_host);
        return false;
    }
    mounted_ = true;
    card_ = card;
    host_ = pins.spi_host;
    std::strcpy(mount_, mount_point);  // length validated before allocating the bus
    ESP_LOGI(kTag, "SD mounted at %s", mount_);
    return true;
}

void SdCard::end() {
    if (!mounted_) return;
    if (esp_vfs_fat_sdcard_unmount(mount_, card_) != ESP_OK) {
        ESP_LOGE(kTag, "unmount failed; retaining ownership for retry");
        return;
    }
    card_ = nullptr;
    mounted_ = false;
    spi_bus_free(host_);
}

bool SdCard::exists(const std::string& path) const {
    if (!mounted_) return false;
    FILE* f = std::fopen(path.c_str(), "rb");
    if (f == nullptr) return false;
    std::fclose(f);
    return true;
}

std::vector<std::uint8_t> SdCard::read_file(const std::string& path, std::size_t max_bytes) {
    std::vector<std::uint8_t> out;
    if (!mounted_) return out;
    FILE* f = std::fopen(path.c_str(), "rb");
    if (f == nullptr) {
        ESP_LOGW(kTag, "open failed: %s", path.c_str());
        return out;
    }
    if (std::fseek(f, 0, SEEK_END) != 0) { std::fclose(f); return out; }
    const long size = std::ftell(f);
    if (std::fseek(f, 0, SEEK_SET) != 0) { std::fclose(f); return out; }
    if (size <= 0 || static_cast<std::size_t>(size) > max_bytes) {
        ESP_LOGW(kTag, "file size %ld rejected (max %zu)", size, max_bytes);
        std::fclose(f);
        return out;
    }
    out.resize(static_cast<std::size_t>(size));
    const std::size_t got = std::fread(out.data(), 1, out.size(), f);
    std::fclose(f);
    if (got != out.size()) {
        out.clear();  // short read => fail closed
    }
    return out;
}

}  // namespace jalak::hal

#endif  // ESP_PLATFORM
