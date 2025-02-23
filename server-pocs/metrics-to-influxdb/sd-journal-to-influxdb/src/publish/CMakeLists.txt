add_library(publish-influxdb publish_influxdb.c publish_influxdb.h)
target_include_directories(publish-influxdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(publish-influxdb systemd influxdb-mqtt)
