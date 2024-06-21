# Add subdirectories
add_subdirectory(utilities)
add_subdirectory(posix-mq-to-mqtt-bridge)
add_subdirectory(inotify-coredump)
add_subdirectory(inotify-pacct)
add_subdirectory(cyclic-file-read)

# Set the output directory for the executable
set_target_properties(cyclic-file-read inotify-coredump inotify-pacct bridge PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)