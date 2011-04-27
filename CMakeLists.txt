# Minimal CMake build file
# Builds:
# - static library from lib directory
# - commandline executable
# - test suite
# - Qt GUI

# To build with CMake:
# - install CMake 2.6 or later
# - $ cmake .
# - $ make

cmake_minimum_required (VERSION 2.6)

PROJECT(CPPCHECK)

set(CMAKE_MODULE_PATH "${CPPCHECK_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
find_package(PCRE)

if(NOT PCRE_FOUND)
   message("User-defined rules are disabled since PCRE libraries were not found.
The PCRE paths can be configured by 'cmake-gui'.")
endif()

# Building lib as static library is disabled due to bug
# #1299 CMake: The CheckClass is not used
# https://sourceforge.net/apps/trac/cppcheck/ticket/1299
# Instead lib code is included directly into cli and gui targets
# ADD_SUBDIRECTORY(lib)

ADD_SUBDIRECTORY(cli)
ADD_SUBDIRECTORY(gui)

ENABLE_TESTING()
ADD_SUBDIRECTORY(test)
