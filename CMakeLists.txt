cmake_minimum_required(VERSION 3.14.0)
project(stats-recording VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
option(STATS_ENABLE_TESTING "Enable tests" ON) # Define an option for enabling tests

# Include helper CMake files
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Policies)
include(ExternalDependencies)

# Add subdirectories
add_subdirectory(src)

if(STATS_ENABLE_TESTING)
    add_subdirectory(test)
endif()

# Include installation and packaging
include(Installation)
include(Packaging)