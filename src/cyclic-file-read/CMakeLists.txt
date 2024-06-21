add_executable(cyclic-file-read
    common.c
    cyclic-file-read.c
    disk.c
    net.c
    pid.c
    stat.c
    sys.c)

target_link_libraries(cyclic-file-read PUBLIC utilities sensors)
target_include_directories(cyclic-file-read PUBLIC ${CMAKE_SOURCE_DIR}/include/cyclic-file-read)
