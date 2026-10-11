# jalak_hal: abstract interfaces + mocks (portable) + IDF-backed drivers.
#
# LOCAL DEMO build: only the OLED, GPIO touch, I2S speaker (TX), and SD drivers
# are registered here. The voice/cloud drivers (esp_http_transport) stay OUT of
# the default build, and the mic (RX) is disabled in app_main.
#
# The IDF driver sources compile to nothing unless ESP_PLATFORM is defined, so
# listing them is safe for host-only usage too.
idf_component_register(
    SRCS
        "src/placeholder.cpp"
        "src/i2c_oled_display.cpp"
        "src/touch_input.cpp"
        "src/i2s_audio_device.cpp"
        "src/sd_card.cpp"
    INCLUDE_DIRS "include"
    REQUIRES jalak_logic esp_driver_i2c esp_driver_i2s esp_driver_spi sdmmc
    PRIV_REQUIRES esp_driver_gpio esp_driver_sdspi fatfs
)
