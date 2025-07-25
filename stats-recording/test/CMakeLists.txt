add_executable(inotify-coredump-test test_inotify_coredump.cpp ${CMAKE_SOURCE_DIR}/src/inotify-coredump/inotify-coredump.c)
add_executable(bridge-test test_bridge.cpp)

target_link_libraries(inotify-coredump-test PUBLIC
    inotify-coredump
    gtest_main
    gmock_main)
target_link_libraries(bridge-test PUBLIC
    gtest_main
    gmock_main
    bridge)

target_include_directories(inotify-coredump-test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(bridge-test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(inotify-coredump-test bridge-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set_target_properties(bridge-test PROPERTIES
    INSTALL_RPATH "${CMAKE_BINARY_DIR}/_deps/paho-mqtt-c-build/src"
    BUILD_WITH_INSTALL_RPATH TRUE
)

gtest_discover_tests(inotify-coredump-test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    EXTRA_ARGS --gtest_color=yes)

gtest_discover_tests(bridge-test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    EXTRA_ARGS --gtest_color=yes)

target_link_options(inotify-coredump-test PRIVATE
    -Wl,--wrap=coredump_to_line_protocol
    -Wl,--wrap=init_mq
    -Wl,--wrap=send_to_mq
    -Wl,--wrap=read
)