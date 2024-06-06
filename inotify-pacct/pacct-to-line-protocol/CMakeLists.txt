add_library(pacct-to-line-protocol pacct_to_line_protocol.c pacct_to_line_protocol.h)
target_include_directories(pacct-to-line-protocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pacct-to-line-protocol pacct utilities)
