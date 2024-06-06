add_library(inotify-pacct inotify_pacct.c inotify_pacct.h)

target_include_directories(inotify-pacct PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})