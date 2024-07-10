# Packaging

# Essential
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "An Hoang <an.hoang@tu-braunschweig.de>")

# Highly Recommended
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C application for sending statistics to Mosquitto")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "gcc, make, cmake, libdbus-1-dev, systemd-coredump, libsensors4-dev, acct")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_ARCH}") # Adjust

# Good Practice
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_PACKAGE_DESCRIPTION "C application for sending statistics to Mosquitto. This application collects system statistics and sends them to a MQTT broker.")
set(CPACK_DEBIAN_PACKAGE_SECTION "base")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# Additional
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/debs")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://gitlab.ibr.cs.tu-bs.de/cm-courses/sep/groups/ss24/cm0/stats-recording")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_BINARY_DIR}/DEBIAN/postinst")

include(CPack)