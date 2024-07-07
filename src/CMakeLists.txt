# Add subdirectories
add_subdirectory(cyclic-file-read)
add_subdirectory(cyclic-sysinfo)
add_subdirectory(dbus-systemd-signals)
add_subdirectory(epoll-psi)
add_subdirectory(inotify-coredump)
add_subdirectory(inotify-pacct)
add_subdirectory(posix-mq-to-mqtt-bridge)
add_subdirectory(utilities)

# Set the output directory for the executable
set_target_properties(cyclic-file-read-exec cyclic-sysinfo-exec dbus-systemd-signals-exec epoll-psi-exec inotify-coredump-exec inotify-pacct-exec bridge-exec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
