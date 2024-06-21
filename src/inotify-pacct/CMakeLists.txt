add_executable(inotify-pacct-exec
    main.c
    inotify-pacct.c
    pacct_to_line_protocol.c
    pacct.c)

target_include_directories(inotify-pacct-exec PUBLIC ${CMAKE_SOURCE_DIR}/include/inotify-pacct)
target_link_libraries(inotify-pacct-exec PUBLIC utilities)
