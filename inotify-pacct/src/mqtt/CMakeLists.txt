add_library(mqtt mqtt.c mqtt.h)
target_include_directories(mqtt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mqtt paho-mqtt3c)