add_library(bridge bridge.c bridge.h)

target_link_libraries(bridge PUBLIC utilities)
target_link_libraries(bridge PRIVATE eclipse-paho-mqtt-c::paho-mqtt3a-static eclipse-paho-mqtt-c::paho-mqtt3c-static eclipse-paho-mqtt-c::paho-mqtt3as-static eclipse-paho-mqtt-c::paho-mqtt3cs-static)

target_include_directories(bridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
