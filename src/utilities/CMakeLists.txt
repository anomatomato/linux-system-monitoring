add_library(utilities
    mq.c
    signal_handling.c
    line_protocol_escaper.c
    line_protocol_parser.c
    file_utils.c
)

target_include_directories(utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})