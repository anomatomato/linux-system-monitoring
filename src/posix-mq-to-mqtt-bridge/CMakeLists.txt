add_executable(bridge-exec bridge.c)

target_include_directories(bridge-exec PUBLIC
    ${CMAKE_SOURCE_DIR}/include/posix-mq-to-mqtt-bridge
    ${PAHO_MQTT_C_INCLUDE_DIR})
target_link_libraries(bridge-exec PUBLIC utilities ${PAHO_MQTT_C_LIBRARIES})