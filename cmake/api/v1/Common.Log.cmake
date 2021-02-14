
if( NOT EBS_CONFIG_ENABLE_LOG )
    set( EBS_CONFIG_ENABLE_LOG TRUE )
endif()

if( NOT EBS_CONFIG_SET_LOG_BREAK_ON_ERROR )
    set( EBS_CONFIG_SET_LOG_BREAK_ON_ERROR TRUE )
endif()

set( EBS_LOG_SEVERITY_LEVEL_ALWAYSON 1 CACHE INTERNAL "" )
set( EBS_LOG_SEVERITY_LEVEL_CRITICAL 2 CACHE INTERNAL "" )
set( EBS_LOG_SEVERITY_LEVEL_ERROR    3 CACHE INTERNAL "" )
set( EBS_LOG_SEVERITY_LEVEL_WARNING  4 CACHE INTERNAL "" )
set( EBS_LOG_SEVERITY_LEVEL_INFO     5 CACHE INTERNAL "" )
set( EBS_LOG_SEVERITY_LEVEL_DBGINFO  6 CACHE INTERNAL "" )
set( EBS_LOG_SEVERITY_LEVEL_DEFAULT  6 CACHE INTERNAL "" )

set( EBS_LOG_SEVERITY_LEVEL EBS_LOG_SEVERITY_LEVEL_DEFAULT )

#
function( ebsLog pLogCategory pSeverityLevel )

    if( NOT EBS_LOG_SEVERITY_LEVEL )
        set( EBS_LOG_SEVERITY_LEVEL EBS_LOG_SEVERITY_LEVEL_DEFAULT )
    endif()

    if( EBS_CONFIG_ENABLE_LOG AND EBS_CONFIG_ENABLE_LOG_CATEGORY_${pLogCategory} )
        if( ${${EBS_LOG_SEVERITY_LEVEL}} GREATER_EQUAL ${${pSeverityLevel}} )
            if( EBS_CONFIG_LOG_CATEGORY_PREFIX_STR_${pLogCategory} )
                set( categoryPrefixStr "[${EBS_LOG_CATEGORY_PREFIX_STR_${pLogCategory}}] " )
            else()
                set( categoryPrefixStr "[${pLogCategory}] " )
            endif()
            if( ( "${${pSeverityLevel}}" MATCHES "${EBS_LOG_SEVERITY_LEVEL_ERROR}|${EBS_LOG_SEVERITY_LEVEL_CRITICAL}" ) AND EBS_CONFIG_SET_LOG_BREAK_ON_ERROR )
                message( FATAL_ERROR "${categoryPrefixStr}" "${ARGN}" )
            else()
                message( STATUS "${categoryPrefixStr}" "${ARGN}" )
            endif()
        endif()
    endif()

endfunction()


#
macro( ebsLogDbginfo pLogCategory )
    ebsLog( ${pLogCategory} EBS_LOG_SEVERITY_LEVEL_DBGINFO "${ARGN}" )
endmacro()
#
macro( ebsLogInfo pLogCategory )
    ebsLog( ${pLogCategory} EBS_LOG_SEVERITY_LEVEL_INFO "${ARGN}" )
endmacro()
#
macro( ebsLogWarning pLogCategory )
    ebsLog( ${pLogCategory} EBS_LOG_SEVERITY_LEVEL_WARNING "${ARGN}" )
endmacro()
#
macro( ebsLogError pLogCategory )
    ebsLog( ${pLogCategory} EBS_LOG_SEVERITY_LEVEL_ERROR "${ARGN}" )
endmacro()
#
macro( ebsLogCritical pLogCategory )
    ebsLog( ${pLogCategory} EBS_LOG_SEVERITY_LEVEL_CRITICAL "${ARGN}" )
endmacro()
#
macro( ebsLogAlwaysOn pLogCategory )
    ebsLog( ${pLogCategory} EBS_LOG_SEVERITY_LEVEL_ALWAYSON "${ARGN}" )
endmacro()
