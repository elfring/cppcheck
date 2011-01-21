cmake_minimum_required(VERSION 2.6)
project(CPPCHECK)
set(CMAKE_MODULE_PATH "${CPPCHECK_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

file(STRINGS "${CPPCHECK_SOURCE_DIR}/lib/library_version.h"
     CPPCHECK_BUILD_SPECIFICATION REGEX "^[ \t]*#define[ \t]+CPPCHECK_LIBRARY_VERSION_[A-Z]+[ \t]+[0-9]+.*$")

if(CPPCHECK_BUILD_SPECIFICATION)
   foreach(item IN ITEMS MAJOR MINOR PATCH)
      set(name "CPPCHECK_LIBRARY_VERSION_${item}")

      if(CPPCHECK_BUILD_SPECIFICATION MATCHES ".*define[ \t]+${name}[ \t]+([0-9]+).*")
         set("CPPCHECK_BUILD_VERSION_${item}" ${CMAKE_MATCH_1} CACHE STRING
             "Version number for the build of the Cppcheck software")
      else()
         message(FATAL_ERROR "A value was not found for the setting \"${name}\" from the build specification.")
      endif()
   endforeach()
else()
   message(FATAL_ERROR "Data were not found for the required build specification.")
endif()

mark_as_advanced(CPPCHECK_BUILD_VERSION_MAJOR CPPCHECK_BUILD_VERSION_MINOR CPPCHECK_BUILD_VERSION_PATCH)

add_definitions(-DCPPCHECK_BUILD_USE_CONFIGURATION_HEADER)
configure_file("${CPPCHECK_SOURCE_DIR}/include/top-config.h.in" "${CPPCHECK_BINARY_DIR}/include/top-config.h")
include_directories("${CPPCHECK_BINARY_DIR}/include")

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
