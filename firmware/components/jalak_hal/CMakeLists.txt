# jalak_hal: abstract interfaces + mocks (portable) + IDF-backed drivers.
#
# The IDF driver sources compile to nothing unless ESP_PLATFORM is defined (their
# bodies are wrapped in `#if defined(ESP_PLATFORM)`), so listing them is safe for
# host-only usage too. They require IDF driver headers only when built by idf.py.
idf_component_register(
    SRCS
        "src/placeholder.cpp"
        "src/i2c_oled_display.cpp"
        "src/touch_input.cpp"
        "src/i2s_audio_device.cpp"
        "src/esp_http_transport.cpp"
    INCLUDE_DIRS "include"
    REQUIRES jalak_logic
    PRIV_REQUIRES driver esp_http_client esp_timer
)
