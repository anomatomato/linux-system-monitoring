cmake_minimum_required(VERSION 3.0.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C)

set(CMAKE_TOOLCHAIN_FILE "/opt/vcpkg/scripts/buildsystems/vcpkg.make" CACHE STRING "Vcpkg toolchain file")

include(CTest)
enable_testing()

# Detect system architecture
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(VCPKG_TARGET_TRIPLET "arm64-linux")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm")
    set(VCPKG_TARGET_TRIPLET "arm-linux")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(VCPKG_TARGET_TRIPLET "x64-linux")
else()
    message(FATAL_ERROR "Unsupported processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

# Set the CMAKE_PREFIX_PATH based on the detected architecture
set(CMAKE_PREFIX_PATH "/opt/vcpkg/installed/${VCPKG_TARGET_TRIPLET}/share")

find_package(eclipse-paho-mqtt-c CONFIG REQUIRED)

add_executable(bridge-exec ${CMAKE_SOURCE_DIR}/posix-mq-to-mqtt-bridge/bridge.c)
add_executable(inotify-coredump-exec ${CMAKE_SOURCE_DIR}/testing/inotify-coredump-test.c)
add_executable(inotify-pacct-exec ${CMAKE_SOURCE_DIR}/testing/inotify-pacct-test.c)

add_subdirectory(utilities)
add_subdirectory(posix-mq-to-mqtt-bridge)
add_subdirectory(inotify-coredump)
add_subdirectory(inotify-pacct)

target_link_libraries(inotify-coredump-exec PRIVATE utilities bridge inotify-coredump)
target_link_libraries(inotify-pacct-exec PRIVATE PRIVATE utilities bridge inotify-pacct)
target_link_libraries(bridge-exec PRIVATE utilities bridge inotify-coredump inotify-pacct)
target_link_libraries(bridge-exec PRIVATE eclipse-paho-mqtt-c::paho-mqtt3a-static eclipse-paho-mqtt-c::paho-mqtt3c-static eclipse-paho-mqtt-c::paho-mqtt3as-static eclipse-paho-mqtt-c::paho-mqtt3cs-static)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
