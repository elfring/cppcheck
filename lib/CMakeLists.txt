# CMake build file to build cppcheck library

if(PCRE_FOUND)
   add_definitions(-DHAVE_RULES)
   include_directories("${PCRE_INCLUDE_DIR}")
endif()

set(CPPCHECK_LIB_DIR "")
include("library_sources.cmake")

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

add_library(checks SHARED ${CPPCHECK_LIB_SOURCES})
add_library(static_checks STATIC ${CPPCHECK_LIB_SOURCES})
set_target_properties(static_checks PROPERTIES OUTPUT_NAME checks)
set_target_properties(static_checks PROPERTIES PREFIX lib)

if(NOT(${CMAKE_MAJOR_VERSION} GREATER 2) AND NOT(${CMAKE_MINOR_VERSION} GREATER 8))
   set_target_properties(checks PROPERTIES CLEAN_DIRECT_OUTPUT 1)
   set_target_properties(static_checks PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif()

if(PCRE_FOUND)
   target_link_libraries(checks ${PCRE_LIBRARIES})
   target_link_libraries(static_checks ${PCRE_LIBRARIES})
endif()
