add_executable(inotify-pacct
    main.c
    inotify-pacct.c
    pacct_to_line_protocol.c
    pacct.c)

target_include_directories(inotify-pacct PUBLIC ${CMAKE_SOURCE_DIR}/include/inotify-pacct)
target_link_libraries(inotify-pacct PUBLIC utilities)
