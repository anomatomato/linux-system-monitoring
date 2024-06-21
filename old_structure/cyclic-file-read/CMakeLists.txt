file(GLOB_RECURSE SRC_FILES src/*.c)
add_library(cyclic-file-read ${SRC_FILES_CFR})
target_link_libraries(cyclic-file-read PRIVATE utilities)
target_include_directories(cyclic-file-read PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
