cmake_minimum_required(VERSION 3.16)

# ESP-IDF project root. Building this file requires ESP-IDF to be installed
# (idf.py). On a machine without ESP-IDF, the portable logic is built and tested
# by firmware/test/ instead — see firmware/README.md.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# The portable logic uses std::optional / std::atomic / std::string_view, so the
# whole project builds as C++17 (default is gnu++17 in ESP-IDF 5.x, but set it
# explicitly so shared headers can't drift).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sijalak_bedas)
