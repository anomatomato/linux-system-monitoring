add_executable(inotify-coredump-exec main.c inotify-coredump.c)

target_include_directories(inotify-coredump-exec PUBLIC ${CMAKE_SOURCE_DIR}/include/inotify-coredump)
target_link_libraries(inotify-coredump-exec PUBLIC utilities)

add_library(inotify-coredump inotify-coredump.c inotify-coredump.h)
target_include_directories(inotify-coredump PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(inotify-coredump PUBLIC utilities)