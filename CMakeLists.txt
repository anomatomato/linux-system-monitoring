cmake_minimum_required(VERSION 3.0.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C)

set(CMAKE_TOOLCHAIN_FILE "~/opt/vcpkg/scripts/buildsystems/vcpkg.make")

include(CTest)
enable_testing()

set(CMAKE_PREFIX_PATH "/opt/vcpkg/installed/x64-linux/share")
find_package(eclipse-paho-mqtt-c CONFIG REQUIRED)

add_executable(stats-recording ${CMAKE_SOURCE_DIR}/main.c)
add_executable(inotify-coredump-test ${CMAKE_SOURCE_DIR}/testing/inotify-coredump-test.c)
add_test(NAME Inotify-Coredump-Test COMMAND inotify-coredump-test)

add_subdirectory(utilities)
add_subdirectory(posix-mq-to-mqtt-bridge)
add_subdirectory(inotify-coredump)
add_subdirectory(inotify-pacct)

target_link_libraries(inotify-coredump-test PRIVATE utilities bridge inotify-coredump)
target_link_libraries(inotify-coredump-test PRIVATE eclipse-paho-mqtt-c::paho-mqtt3a-static eclipse-paho-mqtt-c::paho-mqtt3c-static eclipse-paho-mqtt-c::paho-mqtt3as-static eclipse-paho-mqtt-c::paho-mqtt3cs-static)
target_link_libraries(stats-recording PRIVATE utilities bridge inotify-coredump)
target_link_libraries(stats-recording PRIVATE eclipse-paho-mqtt-c::paho-mqtt3a-static eclipse-paho-mqtt-c::paho-mqtt3c-static eclipse-paho-mqtt-c::paho-mqtt3as-static eclipse-paho-mqtt-c::paho-mqtt3cs-static)

set_target_properties(inotify-coredump-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
