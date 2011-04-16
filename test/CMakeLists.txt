# CMake build file to build cppcheck test suite

set(TINYXML_INCLUDE_DIR "${CPPCHECK_SOURCE_DIR}/externals/tinyxml/")

SET(CHECKTEST_SRCS
  "${CPPCHECK_SOURCE_DIR}/cli/cmdlineparser.cpp"
  "${CPPCHECK_SOURCE_DIR}/cli/cppcheckexecutor.cpp"
  "${CPPCHECK_SOURCE_DIR}/cli/filelister.cpp"
  "${CPPCHECK_SOURCE_DIR}/cli/threadexecutor.cpp"
  "${CPPCHECK_SOURCE_DIR}/cli/pathmatch.cpp"
  options.cpp
  testautovariables.cpp
  testbufferoverrun.cpp
  testcharvar.cpp
  testclass.cpp
  testcmdlineparser.cpp
  testconstructors.cpp
  testcppcheck.cpp
  testdivision.cpp
  testerrorlogger.cpp
  testexceptionsafety.cpp
  testfilelister.cpp
  testincompletestatement.cpp
  testmathlib.cpp
  testmemleak.cpp
  testnullpointer.cpp
  testobsoletefunctions.cpp
  testoptions.cpp
  testother.cpp
  testpath.cpp
  testpathmatch.cpp
  testpostfixoperator.cpp
  testpreprocessor.cpp
  testrunner.cpp
  testsettings.cpp
  testsimplifytokens.cpp
  teststl.cpp
  testsuite.cpp
  testsymboldatabase.cpp
  testthreadexecutor.cpp
  testtoken.cpp
  testtokenize.cpp
  testuninitvar.cpp
  testunusedfunctions.cpp
  testunusedprivfunc.cpp
  testunusedvar.cpp
  "${TINYXML_INCLUDE_DIR}tinystr.cpp"
  "${TINYXML_INCLUDE_DIR}tinyxml.cpp"
  "${TINYXML_INCLUDE_DIR}tinyxmlerror.cpp"
  "${TINYXML_INCLUDE_DIR}tinyxmlparser.cpp")

option(CPPCHECK_TEST_BUILD_WITH_STATIC_BINDING
       "The object code of Cppchecks's library should be included for the
test suite in a static way.")
mark_as_advanced(CPPCHECK_TEST_BUILD_WITH_STATIC_BINDING)

if(CPPCHECK_TEST_BUILD_WITH_STATIC_BINDING)
   option(CPPCHECK_TEST_BUILD_USE_STATIC_LIBRARY
          "The static library file that will be generated for Cppchecks's class library
should be used for the link step of the test suite.")
   mark_as_advanced(CPPCHECK_TEST_BUILD_USE_STATIC_LIBRARY)

   if(CPPCHECK_TEST_BUILD_USE_STATIC_LIBRARY)
      set(CHECK_LIBS static_checks)
   else()
      set(CPPCHECK_LIB_DIR "${CPPCHECK_SOURCE_DIR}/lib/")
      include("${CPPCHECK_LIB_DIR}library_sources.cmake")
      set(CHECKTEST_SRCS ${CHECKTEST_SRCS} ${CPPCHECK_LIB_SOURCES})
   endif()
else()
   set(CHECK_LIBS checks)
endif()

if(WIN32)
   if(NOT CYGWIN)
      # Windows needs additional shlwapi library.
      set(CHECK_LIBS ${CHECK_LIBS} shlwapi)
   endif()
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories("${CPPCHECK_SOURCE_DIR}/lib"
                    "${CPPCHECK_SOURCE_DIR}/cli"
                    "${TINYXML_INCLUDE_DIR}")
add_executable(test ${CHECKTEST_SRCS})
TARGET_LINK_LIBRARIES(test ${CHECK_LIBS})

# Add custom 'make check' -target
# It compiles and runs tests
add_custom_target(check COMMAND test)
add_dependencies(check test)
