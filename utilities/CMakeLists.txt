add_library(utilities mq.h mq.c)

target_include_directories(utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})