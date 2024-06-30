add_executable(bridge-exec bridge.c)

target_include_directories(bridge-exec PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

add_library(bridge bridge.h bridge.c)

target_link_libraries(bridge-exec PUBLIC utilities eclipse-paho-mqtt-c::paho-mqtt3a)