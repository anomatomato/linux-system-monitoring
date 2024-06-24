cmake_minimum_required(VERSION 3.0.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C)

find_package(eclipse-paho-mqtt-c REQUIRED PATHS /usr/local/paho-mqtt/lib/cmake/eclipse-paho-mqtt-c)

add_subdirectory(src)

# Packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
