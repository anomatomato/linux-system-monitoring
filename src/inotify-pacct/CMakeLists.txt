file(GLOB_RECURSE SRC_IP src/*.c)

add_executable(inotify-pacct-exec ${SRC_IP})

target_include_directories(inotify-pacct-exec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(inotify-pacct-exec PUBLIC utilities)
