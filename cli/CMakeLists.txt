# Instructions for building of the cppcheck command line executable
find_package(PCRE REQUIRED)

configure_file("${CPPCHECK_SOURCE_DIR}/include/cli-config.h.in" "${CPPCHECK_BINARY_DIR}/include/cli-config.h")

set(CPPCHECK_CLI_SOURCES
    "cppcheckexecutor.cpp"
    "cmdlineparser.cpp"
    "filelister.cpp"
    "threadexecutor.cpp"
    "main.cpp")

if(WIN32)
   set(CPPCHECK_CLI_SOURCES ${CPPCHECK_CLI_SOURCES}
       "filelister_win32.cpp")
else()
   set(CPPCHECK_CLI_SOURCES ${CPPCHECK_CLI_SOURCES}
       "filelister_unix.cpp")
endif()

option(CPPCHECK_CLI_BUILD_EMBED_LIBRARY_SOURCE_CODE
       "The source files of Cppchecks's library should be directly included
for the compilation of the command line interface.")
mark_as_advanced(CPPCHECK_CLI_BUILD_EMBED_LIBRARY_SOURCE_CODE)

if(CPPCHECK_CLI_BUILD_EMBED_LIBRARY_SOURCE_CODE)
   set(CPPCHECK_LIB_DIR "${CPPCHECK_SOURCE_DIR}/lib/")
   include("${CPPCHECK_LIB_DIR}library_sources.txt")
   set(CPPCHECK_CLI_SOURCES ${CPPCHECK_CLI_SOURCES} ${CPPCHECK_LIB_SOURCES})
   set(CPPCHECK_CLI_LINK_LIBRARIES ${PCRE_LIBRARIES})
else()
   option(CPPCHECK_CLI_BUILD_USE_STATICALLY_LINKED_LIBRARY
          "The static library file that was generated for Cppchecks's class library should be
used for the link step of the command line interface.")
   mark_as_advanced(CPPCHECK_CLI_BUILD_USE_STATICALLY_LINKED_LIBRARY)

   if(CPPCHECK_CLI_BUILD_USE_STATICALLY_LINKED_LIBRARY)
      set(CPPCHECK_CLI_LINK_LIBRARIES libchecks ${PCRE_LIBRARIES})   # static approach
   else()
      set(CPPCHECK_CLI_LINK_LIBRARIES checks ${PCRE_LIBRARIES})   # shared approach
   endif()
endif()

option(CPPCHECK_CLI_BUILD_USE_EXTERNAL_LINKING_FOR_TinyXML
       "The TinyXML software should be linked in from an external library file.
(The source files that are bundled with Cppcheck should not be used.)")
mark_as_advanced(CPPCHECK_CLI_BUILD_USE_EXTERNAL_LINKING_FOR_TinyXML)

if(CPPCHECK_CLI_BUILD_USE_EXTERNAL_LINKING_FOR_TinyXML)
   find_package(TinyXML REQUIRED)
   set(CPPCHECK_CLI_LINK_LIBRARIES ${CPPCHECK_CLI_LINK_LIBRARIES} ${TINYXML_LIBRARIES})
else()
   set(CPPCHECK_CLI_TinyXML_DIR "${CPPCHECK_SOURCE_DIR}/externals/tinyxml/")
   set(CPPCHECK_CLI_SOURCES ${CPPCHECK_CLI_SOURCES}
       "${CPPCHECK_CLI_TinyXML_DIR}tinystr.cpp"
       "${CPPCHECK_CLI_TinyXML_DIR}tinyxml.cpp"
       "${CPPCHECK_CLI_TinyXML_DIR}tinyxmlerror.cpp"
       "${CPPCHECK_CLI_TinyXML_DIR}tinyxmlparser.cpp")
   set(TINYXML_INCLUDE_DIR "${CPPCHECK_CLI_TinyXML_DIR}")
endif()

if(WIN32)
   # Add Windows resource file
   set(CPPCHECK_CLI_SOURCES ${CPPCHECK_CLI_SOURCES} "cppcheck.rc")

   if(NOT CYGWIN)
      # Windows needs additional library.
      set(CPPCHECK_CLI_LINK_LIBRARIES ${CPPCHECK_CLI_LINK_LIBRARIES} shlwapi)
   endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif()

include_directories("${CPPCHECK_SOURCE_DIR}/lib"
                    "${TINYXML_INCLUDE_DIR}"
                    "${PCRE_INCLUDE_DIR}")
add_executable(cppcheck ${CPPCHECK_CLI_SOURCES})
target_link_libraries(cppcheck ${CPPCHECK_CLI_LINK_LIBRARIES})
