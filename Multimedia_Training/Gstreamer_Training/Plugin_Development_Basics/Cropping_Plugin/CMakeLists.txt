cmake_minimum_required(VERSION 3.0)
project(Croptech)

find_package(PkgConfig)
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)

include_directories(${GST_INCLUDE_DIRS} ${GST_VIDEO_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS} ${GST_VIDEO_LIBRARY_DIRS})

# Define the macros that were missing
add_definitions(-DPACKAGE_VERSION="1.0.0")  # Replace with your desired version
add_definitions(-DGST_LICENSE="LGPL")
add_definitions(-DGST_PACKAGE_NAME="Croptech")
add_definitions(-DGST_PACKAGE_ORIGIN="Bhargav Ram Pranav")

add_library(croptech SHARED
    gstcroptech.c
)

target_link_libraries(croptech
    ${GST_LIBRARIES}
    ${GST_VIDEO_LIBRARIES}
)

install(
    TARGETS croptech
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/gstreamer-1.0
)

