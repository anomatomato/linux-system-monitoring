add_library(influxdb-mqtt mqtt.c mqtt.h)
target_include_directories(influxdb-mqtt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(influxdb-mqtt paho-mqtt3c)
