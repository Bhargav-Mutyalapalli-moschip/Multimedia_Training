cmake_minimum_required(VERSION 3.0)
project(teer)
set(PACKAGE_NAME "teer")
set(PACKAGE_VERSION "1.0.0")  # Set the appropriate version number
set(GST_PACKAGE_NAME ${PACKAGE_NAME})
set(GST_PACKAGE_ORIGIN "http://example.com")  # Set your package origin

# Define a license (adjust as needed)
set(GST_LICENSE "LGPL")

# Add definitions for the macros
add_definitions(-DPACKAGE_NAME="${PACKAGE_NAME}")
add_definitions(-DPACKAGE_VERSION="${PACKAGE_VERSION}")
add_definitions(-DGST_PACKAGE_NAME="${GST_PACKAGE_NAME}")
add_definitions(-DGST_PACKAGE_ORIGIN="${GST_PACKAGE_ORIGIN}")
add_definitions(-DGST_LICENSE="${GST_LICENSE}")

find_package(PkgConfig)
pkg_check_modules(GST REQUIRED gstreamer-1.0)

add_library(teer MODULE gstteer.c)

target_include_directories(teer PRIVATE ${GST_INCLUDE_DIRS})


target_link_libraries(teer ${GST_LIBRARIES})

# Set the installation path for the module target
install(TARGETS teer DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/gstreamer-1.0)

