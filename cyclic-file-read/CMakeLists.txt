file(GLOB_RECURSE SRC_FILES src/*.c)
add_library(cyclic-file-read ${SRC_FILES})

target_include_directories(cyclic-file-read PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
