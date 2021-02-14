
#@VarDef EBS_CONFIG_ENABLE_LOG
#@DocStr If TRUE, enables logs during build configuration processing.

#@VarDef EBS_CONFIG_SET_LOG_BREAK_ON_ERROR
#@DocStr If TRUE, emits FATAL_ERROR on error, causing processing to stop.

#@VarDef EBS_CONFIG_LOG_SEVERITY_LEVEL
#@DocStr Current severity level for logs. All logs with lower severity are not displayed.

#@VarDef EBS_CONFIG_ENABLE_LOG_CATEGORY_${CategoryName}
#@DocStr If TRUE, enables logs emited with pLogCategory set to ${CategoryName}.

#@VarDef EBS_CONFIG_LOG_CATEGORY_PREFIX_STR_${CategoryName}
#@DocStr If set, contains a prefix string, displayed before every log entry for ${CategoryName}. Otherwise ${CategoryName} is used.

set( EBS_LOG_SEVERITY_LEVEL_AlwaysOn 9 CACHE INTERNAL "Predefined severity value: AlwaysOn (9)" )
set( EBS_LOG_SEVERITY_LEVEL_Error    7 CACHE INTERNAL "Predefined severity value: Error    (7)" )
set( EBS_LOG_SEVERITY_LEVEL_Warning  5 CACHE INTERNAL "Predefined severity value: Warning  (5)" )
set( EBS_LOG_SEVERITY_LEVEL_Info     3 CACHE INTERNAL "Predefined severity value: Info     (3)" )
set( EBS_LOG_SEVERITY_LEVEL_Debug    1 CACHE INTERNAL "Predefined severity value: Debug    (1)" )
set( EBS_LOG_SEVERITY_LEVEL_Default  5 CACHE INTERNAL "Predefined severity value: Default  (5)" ) # Default = Info

if( NOT EBS_CONFIG_ENABLE_LOG )
    set( EBS_CONFIG_ENABLE_LOG TRUE )
endif()

if( NOT EBS_CONFIG_SET_LOG_BREAK_ON_ERROR )
    set( EBS_CONFIG_SET_LOG_BREAK_ON_ERROR TRUE )
endif()

if( NOT EBS_CONFIG_LOG_SEVERITY_LEVEL )
    set( EBS_CONFIG_LOG_SEVERITY_LEVEL EBS_LOG_SEVERITY_LEVEL_Default )
endif()

#@FunDef ebsCoreLog
#@DocStr Emits log 
function( ebsCoreLog pLogCategory pSeverityLevel pMessage )
    # If either:
    # - logging is completely disabled (EBS_CONFIG_ENABLE_LOG is empty)
    # - logging is disabled for the specific category (pLogCategory is set AND flag variable is defined AND it is set to FALSE)
    # quit the function and do nothing.
    if( NOT EBS_CONFIG_ENABLE_LOG OR ( pLogCategory AND DEFINED EBS_CONFIG_ENABLE_LOG_CATEGORY_${pLogCategory} AND NOT EBS_CONFIG_ENABLE_LOG_CATEGORY_${pLogCategory} ) )
        return()
    endif()
    # Handle unknown severity level.
	if( NOT EBS_LOG_SEVERITY_LEVEL_${pSeverityLevel} )
		set( pSeverityLevel "Default" )
	endif()
    # If the severity is lower than the minimum specified, do not log it.
    if( ${EBS_LOG_SEVERITY_LEVEL_${pSeverityLevel}} LESS ${${EBS_CONFIG_LOG_SEVERITY_LEVEL}} )
        return()
    endif()
    # This is for the prefix thing. It is possible to specify custom prefix for the category to enable some flexibility.
    if( EBS_CONFIG_LOG_CATEGORY_PREFIX_STR_${pLogCategory} )
        set( categoryPrefixStr "[${EBS_LOG_CATEGORY_PREFIX_STR_${pLogCategory}}]" )
    elseif( pLogCategory )
        # By default, the prefix is the category name.
        set( categoryPrefixStr "[${pLogCategory}]" )
    else()
        # If the category is empty, we just print some generic string.
        set( categoryPrefixStr "[Generic]" )
    endif()
    # If BREAK_ON_ERROR is enabled, we print errors using FATAL_ERROR class, causing the script to stop after message().
    if( ( "${pSeverityLevel}" MATCHES "${EBS_LOG_SEVERITY_LEVEL_Error}" ) AND EBS_CONFIG_SET_LOG_BREAK_ON_ERROR )
        message( FATAL_ERROR "${categoryPrefixStr} ${pMessage}" )
    else()
        message( STATUS "${categoryPrefixStr} ${pMessage}" )
    endif()
endfunction()


macro( ebsCoreLogAlwaysOn pLogCategory pMessage )
    ebsCoreLog( "${pLogCategory}" "AlwaysOn" "${pMessage}" )
endmacro()

macro( ebsCoreLogError pLogCategory pMessage )
    ebsCoreLog( "${pLogCategory}" "Error" "${pMessage}" )
endmacro()

macro( ebsCoreLogWarning pLogCategory pMessage )
    ebsCoreLog( "${pLogCategory}" "Warning" "${pMessage}" )
endmacro()

macro( ebsCoreLogInfo pLogCategory pMessage )
    ebsCoreLog( "${pLogCategory}" "Info" "${pMessage}" )
endmacro()

macro( ebsCoreLogDebug pLogCategory pMessage )
    ebsCoreLog( "${pLogCategory}" "Debug" "${pMessage}" )
endmacro()


macro( ebsCoreLogAlwaysOnGeneric pMessage )
    ebsCoreLog( "" "AlwaysOn" "${pMessage}" )
endmacro()

macro( ebsCoreLogErrorGeneric pMessage )
    ebsCoreLog( "" "Error" "${pMessage}" )
endmacro()

macro( ebsCoreLogWarningGeneric pMessage )
    ebsCoreLog( "" "Warning" "${pMessage}" )
endmacro()

macro( ebsCoreLogInfoGeneric pMessage )
    ebsCoreLog( "" "Info" "${pMessage}" )
endmacro()

macro( ebsCoreLogDebugGeneric pMessage )
    ebsCoreLog( "" "Debug" "${pMessage}" )
endmacro()

