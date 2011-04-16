# CMake build file to build cppcheck command line executable

include_directories("${CPPCHECK_SOURCE_DIR}/lib")

if(PCRE_FOUND)
   add_definitions(-DHAVE_RULES)
   set(TINYXML_INCLUDE_DIR "${CPPCHECK_SOURCE_DIR}/externals/tinyxml/")
   include_directories("${PCRE_INCLUDE_DIR}" "${TINYXML_INCLUDE_DIR}")
   set(CHECK_LIBS ${PCRE_LIBRARIES})
endif()

SET(CHECKCLI_SRCS
  cmdlineparser.cpp
  cppcheckexecutor.cpp
  filelister.cpp
  main.cpp
  pathmatch.cpp
  threadexecutor.cpp)

if(PCRE_FOUND)
   set(CHECKCLI_SRCS ${CHECKCLI_SRCS}
       "${TINYXML_INCLUDE_DIR}tinystr.cpp"
       "${TINYXML_INCLUDE_DIR}tinyxml.cpp"
       "${TINYXML_INCLUDE_DIR}tinyxmlerror.cpp"
       "${TINYXML_INCLUDE_DIR}tinyxmlparser.cpp")
endif()

option(CPPCHECK_CLI_BUILD_WITH_STATIC_BINDING
       "The object code of Cppchecks's library should be included for the
command line interface in a static way.")
mark_as_advanced(CPPCHECK_CLI_BUILD_WITH_STATIC_BINDING)

if(CPPCHECK_CLI_BUILD_WITH_STATIC_BINDING)
   option(CPPCHECK_CLI_BUILD_USE_STATIC_LIBRARY
          "The static library file that will be generated for Cppchecks's class library
should be used for the link step of the command line interface.")
   mark_as_advanced(CPPCHECK_CLI_BUILD_USE_STATIC_LIBRARY)

   if(CPPCHECK_CLI_BUILD_USE_STATIC_LIBRARY)
      set(CHECK_LIBS static_checks ${CHECK_LIBS})
   else()
      set(CPPCHECK_LIB_DIR "${CPPCHECK_SOURCE_DIR}/lib/")
      include("${CPPCHECK_LIB_DIR}library_sources.cmake")
      set(CHECKCLI_SRCS ${CHECKCLI_SRCS} ${CPPCHECK_LIB_SOURCES})
   endif()
else()
   set(CHECK_LIBS checks ${CHECK_LIBS})
endif()

if(WIN32)
   # Add Windows resource file
   set(CHECKCLI_SRCS ${CHECKCLI_SRCS} cppcheck.rc)

   if(NOT CYGWIN)
      # Windows needs additional shlwapi library.
      set(CHECK_LIBS ${CHECK_LIBS} shlwapi)
   endif()
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

add_executable(cppcheck ${CHECKCLI_SRCS})
TARGET_LINK_LIBRARIES(cppcheck ${CHECK_LIBS})
