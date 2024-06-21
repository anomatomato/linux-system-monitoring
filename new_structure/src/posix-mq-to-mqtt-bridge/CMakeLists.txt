add_executable(bridge bridge.c)

target_include_directories(bridge PUBLIC ${CMAKE_SOURCE_DIR}/include/posix-mq-to-mqtt-bridge)
target_link_libraries(bridge PUBLIC utilities ${PAHO_MQTT_C_LIBRARIES})