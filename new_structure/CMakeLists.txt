cmake_minimum_required(VERSION 3.0.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C)

# Find the Paho MQTT C library
find_library(PAHO_MQTT_C_LIBRARIES NAMES paho-mqtt3a paho-mqtt3c paho-mqtt3as paho-mqtt3cs PATHS /usr/local/paho-mqtt/lib /usr/local/lib)

if(NOT PAHO_MQTT_C_LIBRARIES)
    message(FATAL_ERROR "Could not find Paho MQTT C library")
endif()

# Add subdirectories
add_subdirectory(src)

# Packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
