# Set the name of the project
project(hello-world C)
# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 3.7)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Tell the compiler where to look for the GTK headers
include_directories(${GTK3_INCLUDE_DIRS})
# Tell the linker where to look for GTK libraries
link_directories(${GTK3_LIBRARY_DIRS})

# Add other GTK flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

# Add target 'ffw-scanner' to be build from the other source files
add_executable(ffw-scanner main.c GladeFFW.glade functions.c functions.h css/global.css)

# Link the target to the GTK+ libraries
target_link_libraries(ffw-scanner ${GTK3_LIBRARIES})