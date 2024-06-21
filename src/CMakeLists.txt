# Add subdirectories
add_subdirectory(utilities)
add_subdirectory(posix-mq-to-mqtt-bridge)
add_subdirectory(inotify-coredump)
add_subdirectory(inotify-pacct)
add_subdirectory(cyclic-file-read)

# Set the output directory for the executable
set_target_properties(cyclic-file-read-exec inotify-coredump-exec inotify-pacct-exec bridge-exec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)