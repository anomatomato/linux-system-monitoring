cmake_minimum_required(VERSION 3.27)
project(cyclic_file_read C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(cyclic_file_read
        include/disk.h
        include/net.h
        include/stat.h
        src/cycle-file-read.c
        src/disk.c
        src/net.c
        src/stat.c
        src/common.c
        include/common.h)
