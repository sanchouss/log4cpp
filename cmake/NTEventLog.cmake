# EventLog DLL generation sub

function(log4cpp_add_nt_eventlog TARGET_LIST_VAR)

    if (NOT WIN32)
        message(WARNING "NT EventLog support is only for WIN32 platforms")
        return()
    endif ()

    #  option(LOG4CPP_NT_EVENTLOG "Build NT EventLog message DLL" ON)

    if (NOT LOG4CPP_NT_EVENTLOG)
        message(STATUS "NT EventLog support disabled (LOG4CPP_NT_EVENTLOG=OFF)")
        return()
    endif ()

    find_program(MC_EXE mc.exe)
    find_program(RC_EXE rc.exe)

    if (NOT MC_EXE OR NOT RC_EXE)
        message(WARNING "NT EventLog support disabled (mc.exe or rc.exe not found)")
        return()
    endif ()

    message(STATUS "NT EventLog support enabled")

    set(NTEVENTLOG_MC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/NTEventLogCategories.mc)

    set(NTEVENTLOG_MC_DIR ${CMAKE_CURRENT_BINARY_DIR}/ntEventLogRes)
    file(MAKE_DIRECTORY ${NTEVENTLOG_MC_DIR})

    set(NTEVENTLOG_MC_HEADER ${NTEVENTLOG_MC_DIR}/NTEventLogCategories.h)
    set(NTEVENTLOG_MC_RC ${NTEVENTLOG_MC_DIR}/NTEventLogCategories.rc)
    set(NTEVENTLOG_MC_RES ${NTEVENTLOG_MC_DIR}/NTEventLogCategories.res)

    # mc.exe -> .h + .rc
    add_custom_command(
            OUTPUT ${NTEVENTLOG_MC_HEADER} ${NTEVENTLOG_MC_RC}
            COMMAND ${MC_EXE}
            -h ${NTEVENTLOG_MC_DIR}
            -r ${NTEVENTLOG_MC_DIR}
            ${NTEVENTLOG_MC_FILE}
            DEPENDS ${NTEVENTLOG_MC_FILE}
            COMMENT "Running mc.exe"
            VERBATIM
    )

    # rc.exe -> .res
    add_custom_command(
            OUTPUT ${NTEVENTLOG_MC_RES}
            COMMAND ${RC_EXE}
            /fo ${NTEVENTLOG_MC_RES}
            ${NTEVENTLOG_MC_RC}
            DEPENDS ${NTEVENTLOG_MC_RC}
            COMMENT "Running rc.exe"
            VERBATIM
    )

    add_custom_target(NTEventLogMC DEPENDS ${NTEVENTLOG_MC_RES})

    add_library(NTEventLogAppenderShared SHARED
            ${CMAKE_CURRENT_SOURCE_DIR}/src/NTEventLogDummy.cpp
            ${NTEVENTLOG_MC_RES}
    )

    add_dependencies(NTEventLogAppenderShared NTEventLogMC)

    set_target_properties(NTEventLogAppenderShared PROPERTIES
            OUTPUT_NAME "NTEventLogAppender"
    )

    target_include_directories(NTEventLogAppenderShared PRIVATE
            ${NTEVENTLOG_MC_DIR}
    )

    target_link_libraries(NTEventLogAppenderShared PRIVATE kernel32)

    # Return target name to caller
    set(${TARGET_LIST_VAR}
            ${${TARGET_LIST_VAR}} NTEventLogAppenderShared
            PARENT_SCOPE)

endfunction()