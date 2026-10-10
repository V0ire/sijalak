cmake_minimum_required(VERSION 3.16)
project(jalak_oled_workstream LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(OLED_SANITIZERS "Host address/undefined-behavior sanitizers" OFF)

get_filename_component(PROJECT_ROOT "../.." ABSOLUTE)
set(COMPONENTS "${PROJECT_ROOT}/firmware/components")
# Compile existing portable interfaces in this isolated build. Do not invoke the
# shared firmware CMake/test tree or install/download anything.
add_library(oled_existing_logic STATIC
    "${COMPONENTS}/jalak_logic/src/expression.cpp"
    "${COMPONENTS}/jalak_logic/src/framebuffer.cpp"
    "${COMPONENTS}/jalak_logic/src/state_machine.cpp"
    "${COMPONENTS}/jalak_logic/src/oled_codec.cpp"
)
target_include_directories(oled_existing_logic PUBLIC
    "${COMPONENTS}/jalak_logic/include" "${COMPONENTS}/jalak_hal/include")

add_library(oled_character STATIC
    "${COMPONENTS}/jalak_oled/src/character.cpp"
    "${COMPONENTS}/jalak_oled/src/adapter.cpp")
target_include_directories(oled_character PUBLIC "${COMPONENTS}/jalak_oled/include")
target_link_libraries(oled_character PUBLIC oled_existing_logic)

add_executable(oled_tests tests.cpp)
target_link_libraries(oled_tests PRIVATE oled_character)
add_executable(oled_render render_preview.cpp)
target_link_libraries(oled_render PRIVATE oled_character)

foreach(target oled_existing_logic oled_character oled_tests oled_render)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    if(OLED_SANITIZERS)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endforeach()

enable_testing()
add_test(NAME oled_contracts COMMAND oled_tests)
