add_library(utilities
    mq.c
    signal_handling.c)

target_include_directories(utilities PUBLIC ${CMAKE_SOURCE_DIR}/include/utilities)