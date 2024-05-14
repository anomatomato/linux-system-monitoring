cmake_minimum_required(VERSION 3.0.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C)

set(CMAKE_TOOLCHAIN_FILE "~/opt/vcpkg/scripts/buildsystems/vckpg.make")

include(CTest)
enable_testing()

set(CMAKE_PREFIX_PATH "/opt/vcpkg/installed/x64-linux/share")
find_package(eclipse-paho-mqtt-c CONFIG REQUIRED)

add_executable(bridge-exec ${CMAKE_SOURCE_DIR}/posix-mq-to-mqtt-bridge/bridge.c)
add_executable(inotify-coredump-exec ${CMAKE_SOURCE_DIR}/testing/inotify-coredump-test.c)

add_subdirectory(utilities)
add_subdirectory(posix-mq-to-mqtt-bridge)
add_subdirectory(inotify-coredump)
add_subdirectory(inotify-pacct)

target_link_libraries(inotify-coredump-exec PRIVATE utilities bridge inotify-coredump)
target_link_libraries(bridge-exec PRIVATE utilities bridge inotify-coredump)
target_link_libraries(bridge-exec PRIVATE eclipse-paho-mqtt-c::paho-mqtt3a-static eclipse-paho-mqtt-c::paho-mqtt3c-static eclipse-paho-mqtt-c::paho-mqtt3as-static eclipse-paho-mqtt-c::paho-mqtt3cs-static)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
