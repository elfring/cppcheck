# Instructions for building of the cppcheck Qt GUI
find_package(Qt4 REQUIRED)

# Add needed Qt modules
set(QT_USE_QTHELP TRUE)
set(QT_USE_QTMAIN TRUE)
set(QT_USE_QTXML TRUE)
include(${QT_USE_FILE})

include_directories (${CPPCHECK_SOURCE_DIR}/lib)
# Generated files (in build directory) need to know gui directory
include_directories (${CPPCHECK_SOURCE_DIR}/gui)

# Header files - listed for mocking
set(CPPCHECK_GUI_HEADERS
    aboutdialog.h
    applicationdialog.h
    applicationlist.h
    checkstatistics.h
    checkthread.h
    common.h
    csvreport.h
    erroritem.h
    filelist.h
    fileviewdialog.h
    helpwindow.h
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
    xmlreport.h)

# Source files
set(CPPCHECK_GUI_SOURCES
    aboutdialog.cpp
    applicationdialog.cpp 
    applicationlist.cpp
    checkstatistics.cpp
    checkthread.cpp
    csvreport.cpp
    erroritem.cpp
    filelist.cpp
    fileviewdialog.cpp
    helpwindow.cpp
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
    xmlreport.cpp)

# User interface files
set(CPPCHECK_UIS
    about.ui
    application.ui
    file.ui
    helpwindow.ui
    logview.ui
    main.ui
    projectfile.ui
    resultsview.ui
    settings.ui
    stats.ui)

# Translation files
set(CPPCHECK_TRANS
    cppcheck_de.ts
    cppcheck_en.ts
    cppcheck_fi.ts
    cppcheck_fr.ts
    cppcheck_ja.ts
    cppcheck_nl.ts
    cppcheck_pl.ts
    cppcheck_su.ts
    cppcheck_se.ts
    cppcheck_sr.ts)

set(CPPCHECK_RCCS gui.qrc)

if(1)
   if(1)
      find_package(PCRE REQUIRED)
      include_directories(${PCRE_INCLUDE_DIR})

      set(CPPCHECK_LIB_DIR ${CPPCHECK_SOURCE_DIR}/lib/)
      include(${CPPCHECK_LIB_DIR}library_sources.txt)
      set(CPPCHECK_GUI_SOURCES ${CPPCHECK_GUI_SOURCES} ${CPPCHECK_LIB_SOURCES})
      set(CPPCHECK_GUI_LINK_LIBRARIES ${PCRE_LIBRARIES})
   else()
      set(CPPCHECK_GUI_LINK_LIBRARIES libchecks)   # static approach
   endif()
else()
   set(CPPCHECK_GUI_LINK_LIBRARIES checks)   # shared approach
endif()

if(WIN32)
   # Add Windows resource file
   SET(CHECK_GUI_SOURCES ${CHECK_GUI_SOURCES} cppcheck-gui.rc)

   if(NOT CYGWIN)
      # Windows needs additional libraries.
      set(CPPCHECK_GUI_LINK_LIBRARIES ${CPPCHECK_GUI_LINK_LIBRARIES} shlwapi htmlhelp)
   endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# Generate rules for building source files from the resources
QT4_ADD_RESOURCES(CPPCHECK_RCC_SRCS ${CPPCHECK_RCCS})

# Process UI files
QT4_WRAP_UI(CPPCHECK_FORMS_HEADERS ${CPPCHECK_UIS})

# Mock header files
QT4_WRAP_CPP(CPPCHECK_MOC_SOURCES ${CPPCHECK_GUI_HEADERS})

QT4_ADD_TRANSLATION(CPPCHECK_QM ${CPPCHECK_TRANS})

# Create folders for Visual Studio IDE
SOURCE_GROUP("Header Files" FILES ${CPPCHECK_GUI_HEADERS})
SOURCE_GROUP("Ui Files" ".ui$")
SOURCE_GROUP("Moc Files" "moc_.*cxx$")

# Include binary directory where code from UI files gets created
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(gui WIN32 ${CPPCHECK_GUI_SOURCES} ${CPPCHECK_MOC_SOURCES} 
               ${CPPCHECK_FORMS_HEADERS} ${CPPCHECK_RCC_SRCS})
TARGET_LINK_LIBRARIES(gui ${CPPCHECK_GUI_LINK_LIBRARIES} ${QT_LIBRARIES})
