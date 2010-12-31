cmake_minimum_required(VERSION 2.6)
project(CPPCHECK)
set(CMAKE_MODULE_PATH ${CPPCHECK_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

add_subdirectory(cli)
add_subdirectory(gui)
add_subdirectory(man)

# Building lib as static library is disabled due to
# bug #1299 CMake: The CheckClass is not used
# https://sourceforge.net/apps/trac/cppcheck/ticket/1299
# Instead lib code is included directly into cli and gui targets
add_subdirectory(lib)

# Exclude tests from command line targets but include them to VS IDE targets.
# There is 'make check' -target for compiling and running tests from
# command line.
if(MSVC_IDE)
   add_subdirectory(test)
else()
   add_subdirectory(test EXCLUDE_FROM_ALL)
endif()

