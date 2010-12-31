# Instructions for building of the cppcheck command line executable
set(CPPCHECK_CLI_SOURCES
    cppcheckexecutor.cpp
    threadexecutor.cpp
    main.cpp)

if(1)
   if(1)
      find_package(PCRE REQUIRED)
      include_directories(${PCRE_INCLUDE_DIR})

      set(CPPCHECK_LIB_DIR ${CPPCHECK_SOURCE_DIR}/lib/)
      include(${CPPCHECK_LIB_DIR}library_sources.txt)
      set(CPPCHECK_CLI_SOURCES ${CPPCHECK_CLI_SOURCES} ${CPPCHECK_LIB_SOURCES})
      set(CPPCHECK_CLI_LINK_LIBRARIES ${PCRE_LIBRARIES})
   else()
      set(CPPCHECK_CLI_LINK_LIBRARIES libchecks)   # static approach
   endif()
else()
   set(CPPCHECK_CLI_LINK_LIBRARIES checks)   # shared approach
endif()

if(WIN32)
   # Add Windows resource file
   set(CPPCHECK_CLI_SOURCES ${CPPCHECK_CLI_SOURCES} cppcheck.rc)

   if(NOT CYGWIN)
      # Windows needs additional library.
      set(CPPCHECK_CLI_LINK_LIBRARIES ${CPPCHECK_CLI_LINK_LIBRARIES} shlwapi)
   endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
       "-Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif()

include_directories (${CPPCHECK_SOURCE_DIR}/lib)
add_executable(cppcheck ${CPPCHECK_CLI_SOURCES})
target_link_libraries(cppcheck ${CPPCHECK_CLI_LINK_LIBRARIES})

