cmake_policy(SET CMP0048 NEW)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
    # Set policy CMP0135 to NEW to handle the DOWNLOAD_EXTRACT_TIMESTAMP feature properly
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW) # surpress warnings for all subprojects