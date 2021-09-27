
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

ebsSysconfInitialize()
