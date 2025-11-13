#
# Nicholas Yue nicholas_yue@users.sourceforge.net
#
# Note:
# (1) A dummy file include/config.h is required (remance from configure)
# (2) Default installation directory is /usr/local, override with -DCMAKE_INSTALL_PREFIX="" during cmake
#     invocation
# (3) Do the usual "make clean all" to build the library
# (4) To install either "make install" or "make install DESTDIR=<your directory>"
# (5) Need to include changes in include/log4cpp/Portability.hh for OSX to build

CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT ( LOG4CPP )
include(GNUInstallDirs)
SET ( LOG4CPP_LIBRARY_NAME "log4cpp" )
SET ( CMAKE_VERBOSE_MAKEFILE ON )

INCLUDE_DIRECTORIES ( include )
INCLUDE_DIRECTORIES ( . )

IF (NOT CMAKE_BUILD_TYPE)
  SET( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
ENDIF ()

IF (WIN32)
  SET ( CMAKE_DEBUG_POSTFIX "D" )
  ADD_DEFINITIONS ( -DWIN32 )
  IF ( NOT CMAKE_BUILD_TYPE STREQUAL "Release" )
    SET ( WIN_DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX} )
  ENDIF()
ELSE (WIN32)
  IF (APPLE)
    ADD_DEFINITIONS ( -DLOG4CPP_HAVE_SSTREAM )
  ELSE (APPLE)
    ADD_DEFINITIONS ( -pthread -DLOG4CPP_HAVE_SSTREAM )
  ENDIF (APPLE)
ENDIF (WIN32)

SET (LOG4CPP_LIBRARY_SOURCES
  src/Appender.cpp
  src/AppenderSkeleton.cpp
  src/AppendersFactory.cpp
  src/BufferingAppender.cpp
  src/FactoryParams.cpp
  src/LayoutsFactory.cpp
  src/LevelEvaluator.cpp
  src/Localtime.cpp
  src/PassThroughLayout.cpp
  src/TriggeringEventEvaluatorFactory.cpp
  src/LayoutAppender.cpp
  src/FileAppender.cpp
  src/DailyRollingFileAppender.cpp
  src/RollingFileAppender.cpp
  src/FixedContextCategory.cpp
  src/IdsaAppender.cpp
  src/OstreamAppender.cpp
  src/StringQueueAppender.cpp
  src/SyslogAppender.cpp
  src/RemoteSyslogAppender.cpp
  src/SimpleLayout.cpp
  src/BasicLayout.cpp
  src/PatternLayout.cpp
  src/Category.cpp
  src/CategoryStream.cpp
  src/HierarchyMaintainer.cpp
  src/Configurator.cpp
  src/BasicConfigurator.cpp
  src/SimpleConfigurator.cpp
  src/PropertyConfigurator.cpp
  src/PropertyConfiguratorImpl.cpp
  src/LoggingEvent.cpp
  src/Priority.cpp
  src/NDC.cpp
  src/Filter.cpp
  src/TimeStamp.cpp
  src/StringUtil.cpp
  src/Properties.cpp
  src/Win32DebugAppender.cpp
  src/NTEventLogAppender.cpp
  src/DllMain.cpp
  src/DummyThreads.cpp
  src/MSThreads.cpp
  src/OmniThreads.cpp
  src/PThreads.cpp
  src/PortabilityImpl.cpp
  src/AbortAppender.cpp
)

# Respect builtin CMake variable BUILD_SHARED_LIBS when it is specified. Otherwise build both static and shared libs
IF (NOT DEFINED BUILD_SHARED_LIBS)
  SET ( LOG4CPP_BUILD_SHARED ON )
  SET ( LOG4CPP_BUILD_STATIC ON )
ELSE()
  IF ( BUILD_SHARED_LIBS )
    SET ( LOG4CPP_BUILD_SHARED ON )
    SET ( LOG4CPP_BUILD_STATIC OFF )
  ELSE ()
    SET ( LOG4CPP_BUILD_SHARED OFF )
    SET ( LOG4CPP_BUILD_STATIC ON )
  ENDIF ()
ENDIF()

MESSAGE("LOG4CPP_BUILD_SHARED is ${LOG4CPP_BUILD_SHARED}; LOG4CPP_BUILD_STATIC is ${LOG4CPP_BUILD_STATIC}")

IF (LOG4CPP_BUILD_STATIC)
  ADD_LIBRARY ( LOG4CPP_STATIC STATIC ${LOG4CPP_LIBRARY_SOURCES} )
  IF (WIN32)
    # win will also build small .lib even when shared library is requested, this is to avoid name clash
    SET_TARGET_PROPERTIES( LOG4CPP_STATIC PROPERTIES OUTPUT_NAME ${LOG4CPP_LIBRARY_NAME}_static )
  ELSE()
    SET_TARGET_PROPERTIES( LOG4CPP_STATIC PROPERTIES OUTPUT_NAME ${LOG4CPP_LIBRARY_NAME} )
  ENDIF()
  TARGET_COMPILE_DEFINITIONS ( LOG4CPP_STATIC PRIVATE $<$<CONFIG:Debug>:_DEBUG DEBUG LOG4CPP_FIX_ERROR_COLLISION> )
  TARGET_COMPILE_DEFINITIONS ( LOG4CPP_STATIC PRIVATE $<$<CONFIG:Release>:NDEBUG> )
ENDIF (LOG4CPP_BUILD_STATIC)

IF (LOG4CPP_BUILD_SHARED)
  ADD_LIBRARY ( LOG4CPP_SHARED SHARED ${LOG4CPP_LIBRARY_SOURCES} )
  SET_TARGET_PROPERTIES( LOG4CPP_SHARED PROPERTIES OUTPUT_NAME ${LOG4CPP_LIBRARY_NAME} )
  TARGET_COMPILE_DEFINITIONS ( LOG4CPP_SHARED PRIVATE $<$<CONFIG:Debug>:_DEBUG DEBUG LOG4CPP_FIX_ERROR_COLLISION> )
  TARGET_COMPILE_DEFINITIONS ( LOG4CPP_SHARED PRIVATE $<$<CONFIG:Release>:NDEBUG> )
  IF (WIN32)
    TARGET_COMPILE_DEFINITIONS ( LOG4CPP_SHARED PRIVATE -D_CRT_SECURE_NO_WARNINGS -DLOG4CPP_HAS_DLL -DLOG4CPP_BUILD_DLL )
  ENDIF (WIN32)
ENDIF (LOG4CPP_BUILD_SHARED)

IF (WIN32)
  # RAD Studio specifics
  IF (CMAKE_CXX_COMPILER MATCHES ".*bcc32c.*" OR CMAKE_CXX_COMPILER MATCHES ".*bcc64x.*")
    MESSAGE(STATUS "Embarcadero C++Builder detected")
    SET ( WIN_DEBUG_POSTFIX "" )
    SET_EMBT_TARGET( LOG4CPP_STATIC RTL )
    SET_EMBT_TARGET( LOG4CPP_SHARED RTL )
  ENDIF()
  
  TARGET_LINK_LIBRARIES ( LOG4CPP_STATIC PRIVATE kernel32 user32 ws2_32 advapi32 msvcrt${WIN_DEBUG_POSTFIX} )
  #SET_TARGET_PROPERTIES ( LOG4CPP_STATIC PROPERTIES LINK_FLAGS /NODEFAULTLIB:msvcrt${WIN_DEBUG_POSTFIX}  )
  TARGET_LINK_LIBRARIES ( LOG4CPP_SHARED PRIVATE kernel32 user32 ws2_32 advapi32 msvcrt${WIN_DEBUG_POSTFIX} )
  #SET_TARGET_PROPERTIES ( LOG4CPP_SHARED PROPERTIES LINK_FLAGS /NODEFAULTLIB:msvcrt${WIN_DEBUG_POSTFIX} )
ENDIF (WIN32)

INSTALL (
  DIRECTORY include/log4cpp
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PATTERN "config.h" EXCLUDE
  PATTERN ".git" EXCLUDE
  PATTERN ".svn" EXCLUDE
  PATTERN "*.am" EXCLUDE
  PATTERN "*.in" EXCLUDE
  )

INSTALL (
  TARGETS ${LOG4CPP_STATIC} ${LOG4CPP_SHARED}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
