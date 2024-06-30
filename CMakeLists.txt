cmake_minimum_required(VERSION 3.14.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(eclipse-paho-mqtt-c REQUIRED PATHS /usr/local/lib/cmake/eclipse-paho-mqtt-c /usr/local/paho-mqtt/lib/cmake/eclipse-paho-mqtt-c)

# Enable Google test
include(FetchContent)
FetchContent_Declare(
    googletest

    # Specify the commit you depend on and update it regularly.
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(test)

# Packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
