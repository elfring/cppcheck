# Instructions for building of the cppcheck test suite
find_package(TinyXML REQUIRED)

set(CPPCHECK_TEST_SOURCES
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
    testincompletestatement.cpp
    testmathlib.cpp
    testmemleak.cpp
    testnullpointer.cpp
    testobsoletefunctions.cpp
    testoptions.cpp
    testother.cpp
    testpath.cpp
    testpostfixoperator.cpp
    testpreprocessor.cpp
    testrunner.cpp
    testsettings.cpp
    testsimplifytokens.cpp
    teststl.cpp
    testsuite.cpp
    testthreadexecutor.cpp
    testtoken.cpp
    testtokenize.cpp
    testuninitvar.cpp
    testunusedfunctions.cpp
    testunusedprivfunc.cpp
    testunusedvar.cpp)

if(1)
   if(1)
      find_package(PCRE REQUIRED)
      include_directories(${PCRE_INCLUDE_DIR})

      set(CPPCHECK_LIB_DIR ${CPPCHECK_SOURCE_DIR}/lib/)
      include(${CPPCHECK_LIB_DIR}library_sources.txt)
      set(CPPCHECK_TEST_SOURCES ${CPPCHECK_TEST_SOURCES} ${CPPCHECK_LIB_SOURCES})
      set(CPPCHECK_TEST_LINK_LIBRARIES ${PCRE_LIBRARIES})
   else()
      set(CPPCHECK_TEST_LINK_LIBRARIES libchecks)   # static approach
   endif()
else()
   set(CPPCHECK_TEST_LINK_LIBRARIES checks)   # shared approach
endif()

if(WIN32 AND NOT CYGWIN)
   # Windows needs additional library.
   set(CPPCHECK_TEST_LINK_LIBRARIES ${CPPCHECK_TEST_LINK_LIBRARIES} shlwapi)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
      "-Wall -Wextra -pedantic -Wshadow -Wno-long-long -Wfloat-equal -Wcast-qual")
endif()

include_directories (${CPPCHECK_SOURCE_DIR}/lib)
add_executable(test ${CPPCHECK_TEST_SOURCES})
target_link_libraries(test ${CPPCHECK_TEST_LINK_LIBRARIES})

# Add custom 'make check' -target
# It compiles and runs tests
add_custom_target(check COMMAND test)
add_dependencies(check test)

