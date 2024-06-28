add_executable(inotify-coredump-test test_inotify_coredump.cpp fixture/InotifyCoredumpFixture.h mock/MockMQ.h)
target_link_libraries(inotify-coredump-test PUBLIC gtest_main gmock_main inotify-coredump utilities)
target_include_directories(inotify-coredump-test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_options(inotify-coredump-test PRIVATE -Wl,--wrap=send_to_mq)

set_target_properties(inotify-coredump-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME inotify-coredump-test COMMAND inotify-coredump-test)
set_tests_properties(inotify-coredump-test PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})