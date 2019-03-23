cmake_minimum_required(VERSION VERSION 3.13)
project(ProjetFFW C)

set(CMAKE_C_STANDARD 99)

#use pkg-config tool to detect gtk header and link libe
find_package(PkgConfig REQUIRED)
PKG_CHECK_MODULES (GTK REQUIRED gtk+-3.0)

#include gtk3 headers
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBE_DIRS})

add_executable(ProjetFFW main.c GladeFFW.glade function.c lib.h)

#add other flags from gtk3
add_definitions(${GTK_GLFAG_OTHER})

#link the target to the gtk+ libraries
target_link_libraries(ProjetFFW ${GTK_LIBRARIES})
#target_link_libraries(ProjetFFW ${GTK_LIBRARIES} -mwindows)