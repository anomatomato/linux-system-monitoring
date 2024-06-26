add_executable(inotify-coredump-test test_inotify_coredump.cpp)
target_link_libraries(inotify-coredump-test PUBLIC gtest_main inotify-coredump utilities)

set_target_properties(inotify-coredump-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME inotify-coredump-test COMMAND inotify-coredump-test)