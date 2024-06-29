add_executable(inotify-coredump-test test_inotify_coredump.cpp)
target_link_libraries(inotify-coredump-test PUBLIC
    gtest_main
    gmock_main
    inotify-coredump
    utilities
    test_utils)
target_include_directories(inotify-coredump-test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_options(inotify-coredump-test PRIVATE -Wl,--wrap=send_to_mq -Wl,--wrap=read)
set_target_properties(inotify-coredump-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

gtest_discover_tests(inotify-coredump-test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    EXTRA_ARGS --gtest_color=yes)

add_subdirectory(test_utils)