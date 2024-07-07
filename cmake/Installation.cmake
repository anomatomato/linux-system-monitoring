# Executables
install(FILES
    ${CMAKE_BINARY_DIR}/bridge-exec
    ${CMAKE_BINARY_DIR}/cyclic-file-read-exec
    ${CMAKE_BINARY_DIR}/inotify-coredump-exec
    ${CMAKE_BINARY_DIR}/inotify-pacct-exec
    ${CMAKE_BINARY_DIR}/epoll-psi-exec
    ${CMAKE_BINARY_DIR}/dbus-systemd-signals-exec
    ${CMAKE_BINARY_DIR}/cyclic-sysinfo-exec
    DESTINATION /usr/local/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# Services
install(FILES
    systemd-services/posix-mq-to-mqtt-bridge.service
    systemd-services/cyclic-file-read.service
    systemd-services/inotify-coredump.service
    systemd-services/inotify-pacct.service
    systemd-services/epoll-psi.service
    systemd-services/dbus-systemd-signals.service
    systemd-services/cyclic-sysinfo.service
    DESTINATION /etc/systemd/system
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# Service Configs
install(FILES
    systemd-services/argconfigs/inotify-coredump.conf
    systemd-services/argconfigs/inotify-pacct.conf
    DESTINATION /etc
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# Manpages
install(FILES
    manpages/cyclic-file-read
    manpages/sd-journal-filter
    DESTINATION /usr/local/share/man/man1
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(FILES
    manpages/posix-mq-to-mqtt-bridge.8
    manpages/inotify-coredump.8
    manpages/inotify-pacct.8
    manpages/epoll-psi.8
    manpages/dbus-systemd-signals.8
    manpages/cyclic-sysinfo.8
    DESTINATION /usr/local/share/man/man8
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
