file(GLOB_RECURSE SRC_FILES_CFR ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_executable(cyclic-file-read-exec ${SRC_FILES_CFR})

target_link_libraries(cyclic-file-read-exec PUBLIC utilities sensors)
target_include_directories(cyclic-file-read-exec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)