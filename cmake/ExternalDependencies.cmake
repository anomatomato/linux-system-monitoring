include(FetchContent)

find_package(DBus1 REQUIRED)
find_package(eclipse-paho-mqtt-c QUIET)

# Paho MQTT C
if(NOT eclipse-paho-mqtt-c_FOUND)
    FetchContent_Declare(
        paho-mqtt-c
        URL https://github.com/eclipse/paho.mqtt.c/archive/refs/tags/v1.3.13.zip
    )
    FetchContent_MakeAvailable(paho-mqtt-c)
    add_library(eclipse-paho-mqtt-c::paho-mqtt3a ALIAS paho-mqtt3a)
endif()

# Google test
if(STATS_ENABLE_TESTING)
    FetchContent_Declare(
        googletest

        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows: Prevent overriding the parent project's compiler/linker settings
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
    enable_testing()
endif()