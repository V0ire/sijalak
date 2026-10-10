# Registration only. Application composition remains owned by the core workstream.
idf_component_register(
    SRCS "src/character.cpp" "src/adapter.cpp"
    INCLUDE_DIRS "include"
    REQUIRES jalak_logic jalak_hal
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)
