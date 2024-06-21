add_executable(inotify-coredump main.c inotify-coredump.c)

target_include_directories(inotify-coredump PUBLIC ${CMAKE_SOURCE_DIR}/include/inotify-coredump)
target_link_libraries(inotify-coredump PUBLIC utilities)