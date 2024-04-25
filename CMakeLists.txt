cmake_minimum_required(VERSION 3.0.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C)

set(CMAKE_TOOLCHAIN_FILE "~/opt/vcpkg/scripts/buildsystems/vckpg.make")

include(CTest)
enable_testing()

set(CMAKE_PREFIX_PATH "/opt/vcpkg/installed/x64-linux/share")
find_package(eclipse-paho-mqtt-c CONFIG REQUIRED)

add_executable(stats-recording main.c)

target_link_libraries(stats-recording PRIVATE eclipse-paho-mqtt-c::paho-mqtt3a-static eclipse-paho-mqtt-c::paho-mqtt3c-static eclipse-paho-mqtt-c::paho-mqtt3as-static eclipse-paho-mqtt-c::paho-mqtt3cs-static)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
