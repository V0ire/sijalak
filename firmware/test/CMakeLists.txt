# Host build + test for the portable jalak_logic library.
# Requires only a C++20 compiler and CMake. No ESP-IDF, no hardware, no network.
cmake_minimum_required(VERSION 3.16)
project(jalak_logic_host_tests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(jalak_logic STATIC
  ../components/jalak_logic/src/expression.cpp
  ../components/jalak_logic/src/framebuffer.cpp
  ../components/jalak_logic/src/state_machine.cpp
  ../components/jalak_logic/src/audio.cpp
  ../components/jalak_logic/src/api_client.cpp
  ../components/jalak_logic/src/oled_codec.cpp
)
target_include_directories(jalak_logic PUBLIC
  ../components/jalak_logic/include
)

enable_testing()

add_executable(test_jalak_logic test_main.cpp)
target_link_libraries(test_jalak_logic PRIVATE jalak_logic)
add_test(NAME jalak_logic COMMAND test_jalak_logic)

# HAL + mock tests include headers from jalak_hal (interface + mock only).
add_executable(test_jalak_hal test_hal.cpp)
target_link_libraries(test_jalak_hal PRIVATE jalak_logic)
target_include_directories(test_jalak_hal PRIVATE
  ../components/jalak_hal/include
)
add_test(NAME jalak_hal COMMAND test_jalak_hal)

# Device API client contract, driven by the mock transport (no network).
add_executable(test_jalak_api test_api.cpp)
target_link_libraries(test_jalak_api PRIVATE jalak_logic)
target_include_directories(test_jalak_api PRIVATE
  ../components/jalak_hal/include
)
add_test(NAME jalak_api COMMAND test_jalak_api)

# Cross-boundary: firmware parser consumes backend-generated wire JSON.
add_executable(test_jalak_integration test_integration.cpp)
target_link_libraries(test_jalak_integration PRIVATE jalak_logic)
target_include_directories(test_jalak_integration PRIVATE
  ../components/jalak_hal/include
)
add_test(NAME jalak_integration COMMAND test_jalak_integration)

# Portable OLED command/page encoder (SSD1306 + SH1106).
add_executable(test_jalak_oled test_oled.cpp)
target_link_libraries(test_jalak_oled PRIVATE jalak_logic)
add_test(NAME jalak_oled COMMAND test_jalak_oled)
