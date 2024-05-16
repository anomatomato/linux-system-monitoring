add_library(inotify-pacct inotify_pacct.h inotify_pacct.c)

target_include_directories(inotify-pacct PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})