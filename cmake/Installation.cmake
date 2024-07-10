# Executables
install(TARGETS
    bridge-exec
    cyclic-file-read-exec
    inotify-coredump-exec
    inotify-pacct-exec
    epoll-psi-exec
    dbus-systemd-signals-exec
    cyclic-sysinfo-exec
    demo-exec
    DESTINATION /usr/local/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# Scripts of Log-Recording
install(FILES
    journal-filter/journal-filter.sh
    journal-filter/journal-upload.sh
    journal-filter/journal-config.sh
    DESTINATION /usr/local/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# install(FILES
# ${CMAKE_BINARY_DIR}/bridge-exec
# ${CMAKE_BINARY_DIR}/cyclic-file-read-exec
# ${CMAKE_BINARY_DIR}/inotify-coredump-exec
# ${CMAKE_BINARY_DIR}/inotify-pacct-exec
# ${CMAKE_BINARY_DIR}/epoll-psi-exec
# ${CMAKE_BINARY_DIR}/dbus-systemd-signals-exec
# ${CMAKE_BINARY_DIR}/cyclic-sysinfo-exec
# DESTINATION /usr/local/bin
# PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
# )

# Services
install(FILES
    systemd-services/posix-mq-to-mqtt-bridge.service
    systemd-services/cyclic-file-read.service
    systemd-services/inotify-coredump.service
    systemd-services/inotify-pacct.service
    systemd-services/epoll-psi.service
    systemd-services/dbus-systemd-signals.service
    systemd-services/cyclic-sysinfo.service
    systemd-services/sd-journal-filter.service
    DESTINATION /etc/systemd/system
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# Service Configs
#install(FILES
    #systemd-services/argconfigs/cyclic-file-read.conf
    #DESTINATION /etc
    #PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
#)

# Manpages
install(FILES
    manpages/cyclic-file-read.8
    manpages/posix-mq-to-mqtt-bridge.8
    manpages/inotify-coredump.8
    manpages/inotify-pacct.8
    manpages/epoll-psi.8
    manpages/dbus-systemd-signals.8
    manpages/cyclic-sysinfo.8
    manpages/sd-journal-filter.8
    manpages/sd-journal-to-influxdb.8
    manpages/coredump-to-influxdb.8
    manpages/stats-recording.8
    DESTINATION /usr/local/share/man/man8
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
