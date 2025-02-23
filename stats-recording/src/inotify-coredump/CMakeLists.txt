add_executable(inotify-coredump-exec main.c)
target_link_libraries(inotify-coredump-exec PUBLIC utilities inotify-coredump)

add_library(inotify-coredump inotify-coredump.c)
target_include_directories(inotify-coredump PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(inotify-coredump PUBLIC utilities)