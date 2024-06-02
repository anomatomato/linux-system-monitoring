add_executable(${PROJECT_NAME} inotify_pacct.c)
target_link_libraries(${PROJECT_NAME} mqtt)
