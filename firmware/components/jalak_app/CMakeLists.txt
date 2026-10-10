# jalak_app: ESP-IDF glue. All code is guarded by #if defined(ESP_PLATFORM), so
# this component contributes nothing to a host build. No public headers.
idf_component_register(
    SRCS "src/app_main.cpp"
    REQUIRES jalak_logic jalak_hal
)
