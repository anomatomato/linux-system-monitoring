cmake_minimum_required(VERSION 3.14.0)
cmake_policy(SET CMP0048 NEW)
project(stats-recording VERSION 0.1.0 LANGUAGES C CXX)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
    # Set policy CMP0135 to NEW to handle the DOWNLOAD_EXTRACT_TIMESTAMP feature properly
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW) # surpress warnings for all subprojects
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

option(STATS_ENABLE_TESTING "Enable tests" ON) # Define an option for enabling tests

install(FILES build/bridge-exec DESTINATION /usr/local/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(FILES build/cyclic-file-read-exec DESTINATION /usr/local/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(FILES build/inotify-coredump-exec DESTINATION /usr/local/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(FILES build/inotify-pacct-exec DESTINATION /usr/local/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(FILES systemd-services/posix-mq-to-mqtt-bridge.service DESTINATION /etc/systemd/system)
install(FILES systemd-services/cyclic-file-read.service DESTINATION /etc/systemd/system)
install(FILES systemd-services/inotify-coredump.service DESTINATION /etc/systemd/system)
install(FILES systemd-services/inotify-pacct.service DESTINATION /etc/systemd/system)

install(FILES systemd-services/argconfigs/cyclic-file-read.conf DESTINATION /etc)
install(FILES systemd-services/argconfigs/inotify-coredump.conf DESTINATION /etc)

install(FILES manpages/posix-mq-to-mqtt-bridge DESTINATION /usr/local/share/man/man1)
install(FILES manpages/cyclic-file-read DESTINATION /usr/local/share/man/man1)
install(FILES manpages/sd-journal-filter DESTINATION /usr/local/share/man/man1)
install(FILES manpages/inotify-coredump.8 DESTINATION /usr/local/share/man/man8)

include(FetchContent)

find_package(eclipse-paho-mqtt-c QUIET)

if(NOT eclipse-paho-mqtt-c_FOUND)
    # Paho MQTT C library was not found on the system, fetch from source
    FetchContent_Declare(
        paho-mqtt-c
        URL https://github.com/eclipse/paho.mqtt.c/archive/refs/tags/v1.3.13.zip
    )
    FetchContent_MakeAvailable(paho-mqtt-c)
    add_library(eclipse-paho-mqtt-c::paho-mqtt3a ALIAS paho-mqtt3a)
endif()

if(STATS_ENABLE_TESTING)
    # Enable Google test only if tests are enabled
    FetchContent_Declare(
        googletest

        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows: Prevent overriding the parent project's compiler/linker settings
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(src)

# Packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# set(CPACK_GENERATOR "DEB")
# set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name") # required
# set(CPACK_PACKAGE_DESCRIPTION "Your package description.")
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A concise version of the description.")
# set(CPACK_DEBIAN_PACKAGE_VERSION "0.1.0") # Set the version of your package
# set(CPACK_PACKAGE_CONTACT "Your Contact Information")
# set(CPACK_DEBIAN_PACKAGE_DEPENDS "libdependency1, libdependency2 (>= version)")
#
# # Optionally specify architecture (if not specified, it defaults to the current architecture)
# set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
#
# # Set additional Debian-specific control files if necessary
# set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
# "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;
# ${CMAKE_CURRENT_SOURCE_DIR}/debian/postrm;
# ${CMAKE_CURRENT_SOURCE_DIR}/debian/preinst;
# ${CMAKE_CURRENT_SOURCE_DIR}/debian/prerm")
#
# include(CPack)
#
#