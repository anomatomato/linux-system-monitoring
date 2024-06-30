add_executable(bridge-exec main.c)
add_library(bridge bridge.h bridge.c)

target_link_libraries(bridge PUBLIC utilities eclipse-paho-mqtt-c::paho-mqtt3a)
target_link_libraries(bridge-exec PUBLIC bridge)

target_include_directories(bridge PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})