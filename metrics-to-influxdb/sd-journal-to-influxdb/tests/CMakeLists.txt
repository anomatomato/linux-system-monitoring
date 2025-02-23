# GoogleTest setup
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
    sd_journal_to_influxdb_test
    journal_to_influxdb_test.cc
    fixtures.h
    utils.h
)
target_link_libraries(
    sd_journal_to_influxdb_test
    GTest::gtest_main
    sd-journal-to-influxdb
)

include(GoogleTest)
gtest_discover_tests(sd_journal_to_influxdb_test)

# Copy data files to the build directory
add_custom_target(copy_data_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/tests/data
    ${CMAKE_BINARY_DIR}/tests/data
)
