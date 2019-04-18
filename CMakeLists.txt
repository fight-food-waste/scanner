# Set the name of the project
project(hello-world C)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 3.7)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# Use the package PkgConfig to detect headers/library files
find_package(PkgConfig REQUIRED)

## GTK ##
# Check if GTK is present
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
# Tell the compiler where to look for the GTK headers
include_directories(${GTK3_INCLUDE_DIRS})
# Tell the linker where to look for GTK libraries
link_directories(${GTK3_LIBRARY_DIRS})
# Add other GTK flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

## GLib ##
# Check if GLib is present
pkg_check_modules(GLIB glib-2.0)
# Tell the compiler where to look for the GLib headers
include_directories(${GLIB_INCLUDE_DIRS})
# Tell the linker where to look for GLib libraries
link_directories(${GLIB_LIBRARY_DIRS})
# Add other GLib flags to the compiler
add_definitions(${GLIB_CFLAGS_OTHER})

## CURL ##
# Load CURL
find_package(CURL REQUIRED)
# Tell the compiler where to look for the CURL headers
include_directories(${CURL_INCLUDE_DIR})

## Jansson ##
# Import Jansson paths
find_path(JANSSON_INCLUDE_DIR jansson.h /usr/include /usr/local/include)
find_library(JANSSON_LIBRARY NAMES jansson PATHS /usr/lib /usr/local/lib)
# Set some variables for cleanliness
set(JANSSON_LIBRARIES ${JANSSON_LIBRARY})
set(JANSSON_INCLUDE_DIRS ${JANSSON_INCLUDE_DIR})
# Tell the compiler where to look for the Jansson headers
include_directories(${JANSSON_INCLUDE_DIR})

# Add target 'ffw-scanner' to be build from the other source files
add_executable(ffw-scanner main.c gui.glade functions.c functions.h css/global.css)

# Link the target to the GTK+ library
target_link_libraries(ffw-scanner ${GTK3_LIBRARIES})
# Link the target to the CURL library
target_link_libraries(ffw-scanner ${CURL_LIBRARIES})
# Link the target to the Jansson library
target_link_libraries(ffw-scanner ${JANSSON_LIBRARIES})
