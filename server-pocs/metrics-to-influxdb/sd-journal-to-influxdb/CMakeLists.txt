cmake_minimum_required(VERSION 3.5.0)

project(journal-to-influxdb)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Disable paho tests
set(PAHO_ENABLE_TESTING FALSE CACHE BOOL "Enable testing for Paho MQTT" FORCE)

# Enable ctest
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(paho.mqtt.c)
