cmake_minimum_required(VERSION 3.14.0)
cmake_policy(SET CMP0048 NEW)
project(stats-recording VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the default CMP0048 policy to NEW for all subprojects to surpress warnings
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

# find_package(eclipse-paho-mqtt-c REQUIRED PATHS /usr/local/lib/cmake/eclipse-paho-mqtt-c /usr/local/paho-mqtt/lib/cmake/eclipse-paho-mqtt-c)
include(FetchContent)

# Enable Google test
FetchContent_Declare(
    googletest

    # Specify the commit you depend on and update it regularly.
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Enable paho.mqtt.c
FetchContent_Declare(
    paho-mqtt-c

    # Specify the commit you depend on and update it regularly.
    URL https://github.com/eclipse/paho.mqtt.c/archive/refs/tags/v1.3.13.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Disable tests for paho.mqtt.c
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest paho-mqtt-c)

include(GoogleTest)
include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(test)

# Packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
