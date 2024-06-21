add_library(inotify-coredump inotify-coredump.c inotify-coredump.h)

target_link_libraries(inotify-coredump PRIVATE utilities)
target_include_directories(inotify-coredump PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})