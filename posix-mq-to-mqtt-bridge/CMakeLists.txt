add_library(bridge bridge.c bridge.h)

target_link_libraries(bridge PUBLIC utilities ${PAHO_MQTT_C_LIBRARIES})

# target_link_libraries(bridge PUBLIC eclipse-paho-mqtt-c::paho-mqtt3c-static eclipse-paho-mqtt-c::paho-mqtt3as-static eclipse-paho-mqtt-c::paho-mqtt3cs-static)
target_include_directories(bridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
