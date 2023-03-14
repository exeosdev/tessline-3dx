
include( "cmake/CMakeApi.Core.cmake" )
include( "cmake/CMakeApi.Sysconf.cmake" )
include( "cmake/CMakeApi.Workspace.cmake" )

function( setDllDependencyCopyCommand pExeTarget pDllTarget )
    add_custom_command( TARGET ${pExeTarget} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${pDllTarget}>" $<TARGET_FILE_DIR:${pExeTarget}>)
endfunction()

function( setExeTargetLibraryDependencies pExeTarget pDllDependencyTargetList )
    foreach( dllTarget ${pDllDependencyTargetList} )
        target_link_libraries( ${pExeTarget} PUBLIC ${dllTarget} )
		setDllDependencyCopyCommand( ${pExeTarget} ${dllTarget} )
    endforeach()
endfunction()

function( addTarget targetName )

    cmake_parse_arguments( IN_ARG "EXE DLL LIB" "" "SOURCES LIBS" "${ARGN}" )

    if( NOT targetName )
        message( FATAL_ERROR "Cannot add a target with empty name." )
    endif()

    if( NOT IN_ARG_SOURCES )
        message( FATAL_ERROR "Cannot add a target without any source files specified ('SOURCES' opt arg is empty)." )
    endif()

    foreach( sourceFileName ${IN_ARG_SOURCES} )
        set( targetSources "${targetSources};${sourceFileName}" )
    endforeach()

    if( IN_ARG_LIBS )
        foreach( libName ${IN_ARG_LIBS} )
            set( targetLibs "${targetLibs};${libName}" )
        endforeach()
    endif()

    if( IN_ARG_EXE )
        add_executable( ${targetName} "${targetSources}" )
    elseif( IN_ARG_DLL )
        add_library( ${targetName} SHARED "${targetSources}" )
    elseif( IN_ARG_LIB )
        add_library( ${targetName} STATIC "${targetSources}" )
    else()
        message( FATAL_ERROR "Target type has not been specified (EXE/DLL/LIB)." )
    endif()

    if( targetLibs )
        target_link_libraries( ${targetName} "${targetLibs}" )
    endif()

endfunction()

ebsSysconfInitialize()
