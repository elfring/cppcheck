# CMake build file to build cppcheck Qt GUI

# find and setup Qt4 for this project
find_package(Qt4)

IF(QT4_FOUND)

# Add needed Qt modules
set(QT_USE_QTHELP TRUE)
set(QT_USE_QTMAIN TRUE)
set(QT_USE_QTXML TRUE)
include(${QT_USE_FILE})

include_directories("${CPPCHECK_SOURCE_DIR}/lib"
# Generated files (in build directory) need to know gui directory
                    "${CPPCHECK_SOURCE_DIR}/gui"
# Include binary directory where code from UI files gets created
                    "${CMAKE_CURRENT_BINARY_DIR}")

# Header files - listed for mocking
SET(CHECK_HEADERS
  aboutdialog.h
  application.h
  applicationdialog.h
  applicationlist.h
  checkstatistics.h
  checkthread.h
  common.h
  csvreport.h
  erroritem.h
  filelist.h
  fileviewdialog.h
  logview.h
  mainwindow.h
  project.h
  projectfile.h
  projectfiledialog.h
  report.h
  resultstree.h
  resultsview.h
  settingsdialog.h
  statsdialog.h
  threadhandler.h
  threadresult.h
  translationhandler.h
  txtreport.h
  xmlreport.h
  xmlreportv1.h
  xmlreportv2.h
)

# Source files
SET(CHECKGUI_SRCS
  aboutdialog.cpp
  application.cpp
  applicationdialog.cpp
  applicationlist.cpp
  checkstatistics.cpp
  checkthread.cpp
  csvreport.cpp
  erroritem.cpp
  filelist.cpp
  fileviewdialog.cpp
  logview.cpp
  main.cpp
  mainwindow.cpp
  project.cpp
  projectfile.cpp
  projectfiledialog.cpp
  report.cpp
  resultstree.cpp
  resultsview.cpp
  settingsdialog.cpp
  statsdialog.cpp
  threadhandler.cpp
  threadresult.cpp
  translationhandler.cpp
  txtreport.cpp
  xmlreport.cpp
  xmlreportv1.cpp
  xmlreportv2.cpp
)

# UI files
SET(CHECK_UIS
  about.ui
  application.ui
  file.ui
  logview.ui
  main.ui
  projectfile.ui
  resultsview.ui
  settings.ui
  stats.ui
)

# Translation files
SET(CHECK_TRANS
  cppcheck_de.ts
  cppcheck_en.ts
  cppcheck_es.ts
  cppcheck_fi.ts
  cppcheck_fr.ts
  cppcheck_ja.ts
  cppcheck_nl.ts
  cppcheck_pl.ts
  cppcheck_ru.ts
  cppcheck_sr.ts
  cppcheck_sv.ts
)

SET(CHECK_RCCS gui.qrc)

option(CPPCHECK_GUI_BUILD_WITH_STATIC_BINDING
       "The object code of Cppchecks's library should be included for the
graphical user interface in a static way.")
mark_as_advanced(CPPCHECK_GUI_BUILD_WITH_STATIC_BINDING)

if(CPPCHECK_GUI_BUILD_WITH_STATIC_BINDING)
   option(CPPCHECK_GUI_BUILD_USE_STATIC_LIBRARY
          "The static library file that will be generated for Cppchecks's class library
should be used for the link step of the graphical user interface.")
   mark_as_advanced(CPPCHECK_GUI_BUILD_USE_STATIC_LIBRARY)

   if(CPPCHECK_GUI_BUILD_USE_STATIC_LIBRARY)
      set(CHECK_LIBS static_checks)
   else()
      set(CPPCHECK_LIB_DIR "${CPPCHECK_SOURCE_DIR}/lib/")
      include("${CPPCHECK_LIB_DIR}library_sources.cmake")
      set(CHECKGUI_SRCS ${CHECKGUI_SRCS} ${CPPCHECK_LIB_SOURCES})
   endif()
else()
   set(CHECK_LIBS checks)
endif()

if(WIN32)
   # Add Windows resource file
   set(CHECKGUI_SRCS ${CHECKGUI_SRCS} cppcheck-gui.rc)

   if(NOT CYGWIN)
      # Windows needs additional shlwapi library.
      set(CHECK_LIBS ${CHECK_LIBS} shlwapi)
   endif()
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCXX)

# Generate rules for building source files from the resources
QT4_ADD_RESOURCES(CHECK_RCC_SRCS ${CHECK_RCCS})

# Process UI files
QT4_WRAP_UI(CHECK_UIS_H ${CHECK_UIS})

# Mock header files
QT4_WRAP_CPP(CHECK_MOC_SRCS ${CHECK_HEADERS})

# add translations ...
QT4_ADD_TRANSLATION(CHECK_QM ${CHECK_TRANS})

# Create folders for Visual Studio IDE
SOURCE_GROUP("Header Files" FILES ${CHECK_HEADERS})
SOURCE_GROUP("Ui Files" ".ui$")
SOURCE_GROUP("Moc Files" "moc_.*cxx$")

ADD_EXECUTABLE(cppcheck-gui WIN32 ${CHECKGUI_SRCS} ${CHECK_MOC_SRCS} ${CHECK_HEADERS} 
               ${CHECK_UIS_H} ${CHECK_RCC_SRCS})
TARGET_LINK_LIBRARIES(cppcheck-gui ${CHECK_LIBS} ${QT_LIBRARIES})

ELSE(QT4_FOUND)
message("GUI not built since QT4 not found.")
ENDIF(QT4_FOUND)
