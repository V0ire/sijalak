# LOCAL ONLY. legacy_voice.cpp is deliberately excluded; no voice opt-in.
idf_component_register(
    SRCS "src/app_main.cpp"
    REQUIRES jalak_local jalak_hal esp_timer
)
