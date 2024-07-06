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