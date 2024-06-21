add_executable(bridge bridge.c)

target_include_directories(bridge PUBLIC
    ${CMAKE_SOURCE_DIR}/include/posix-mq-to-mqtt-bridge
    ${PAHO_MQTT_C_INCLUDE_DIR})
target_link_libraries(bridge PUBLIC utilities ${PAHO_MQTT_C_LIBRARIES})