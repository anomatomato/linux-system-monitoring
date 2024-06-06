cmake_minimum_required(VERSION 3.0.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C)

# Find the Paho MQTT C library
find_path(PAHO_MQTT_C_INCLUDE_DIR NAMES MQTTAsync.h PATHS /usr/local/paho-mqtt/include /usr/local/include)
find_library(PAHO_MQTT_C_LIBRARIES NAMES paho-mqtt3a paho-mqtt3c paho-mqtt3as paho-mqtt3cs PATHS /usr/local/paho-mqtt/lib /usr/local/lib)

if(NOT PAHO_MQTT_C_INCLUDE_DIR OR NOT PAHO_MQTT_C_LIBRARIES)
    message(FATAL_ERROR "Could not find Paho MQTT C library")
endif()

include_directories(${PAHO_MQTT_C_INCLUDE_DIR})

# Add executables
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/cyclic-file-read/src/*.c)

add_executable(bridge-exec ${CMAKE_SOURCE_DIR}/posix-mq-to-mqtt-bridge/bridge.c)
add_executable(inotify-coredump-exec ${CMAKE_SOURCE_DIR}/testing/inotify-coredump-test.c)
add_executable(cyclic-file-read-exec ${SRC_FILES})

# Add subdirectories
add_subdirectory(utilities)
add_subdirectory(posix-mq-to-mqtt-bridge)
add_subdirectory(inotify-coredump)
add_subdirectory(inotify-pacct)
add_subdirectory(cyclic-file-read)

# Link libraries
target_link_libraries(inotify-coredump-exec PRIVATE utilities bridge inotify-coredump)
target_link_libraries(cyclic-file-read-exec PRIVATE utilities bridge cyclic-file-read sensors)
target_link_libraries(bridge-exec PRIVATE utilities ${PAHO_MQTT_C_LIBRARIES})

# target_link_libraries(bridge-exec PRIVATE utilities bridge inotify-coredump inotify-pacct eclipse-paho-mqtt-c::paho-mqtt3a-static eclipse-paho-mqtt-c::paho-mqtt3c-static eclipse-paho-mqtt-c::paho-mqtt3as-static eclipse-paho-mqtt-c::paho-mqtt3cs-static)

# Packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
