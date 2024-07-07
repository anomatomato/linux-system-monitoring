add_executable(dbus-systemd-signals-exec dbus-systemd-signals.c)
target_link_libraries(dbus-systemd-signals-exec PUBLIC utilities dbus-systemd-signals)

add_library(dbus-systemd-signals dbus-systemd-signals.c)
target_include_directories(dbus-systemd-signals PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dbus-systemd-signals PUBLIC utilities systemd)
