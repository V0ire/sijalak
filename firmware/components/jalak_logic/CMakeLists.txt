cmake_minimum_required(VERSION 3.16)
idf_component_register(
    SRCS
        "src/expression.cpp"
        "src/framebuffer.cpp"
        "src/state_machine.cpp"
        "src/audio.cpp"
        "src/api_client.cpp"
        "src/oled_codec.cpp"
    INCLUDE_DIRS "include"
)
