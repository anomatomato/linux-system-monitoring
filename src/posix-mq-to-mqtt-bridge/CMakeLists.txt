add_executable(bridge-exec bridge.c)

target_include_directories(bridge-exec PUBLIC
    ${CMAKE_SOURCE_DIR}/include/posix-mq-to-mqtt-bridge)

target_link_libraries(bridge-exec PUBLIC utilities eclipse-paho-mqtt-c::paho-mqtt3a)