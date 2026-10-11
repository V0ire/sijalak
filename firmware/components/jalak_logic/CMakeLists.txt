cmake_minimum_required(VERSION 3.16)
idf_component_register(
    SRCS
        "src/expression.cpp"
        "src/framebuffer.cpp"
        "src/state_machine.cpp"
        "src/audio.cpp"
        # The local target has no API/voice caller. Keep the exception-based
        # API parser in the separate host build, not the no-exceptions IDF build.
        "src/oled_codec.cpp"
    INCLUDE_DIRS "include"
)
