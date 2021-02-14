
function( ebsGenSpecificAddPrecompiledHeader pProjectID pProjectPath pSourceFileList outPchFileName )

    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "PchActive" isPCHEnabledForProject )

    if( NOT isPCHEnabledForProject )
        ebsLogGenInternalWarning( "PCH is not enabled for project ${pProjectID}. PCH setup ignored" )
        return()
    endif()

    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "PchFileHeader" pchHeaderName )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "PchFileSource" pchSourceName )

    if ( NOT pchHeaderName )
        ebsLogGenInternalWarning( "Missing PCH definition for project ${pProjectID}. PCH setup ignored" )
        return()
    endif()

    set( pchHeaderFile "${pProjectPath}/${pchHeaderName}" )
    set( pchSourceFile "${pProjectPath}/${pchSourceName}" )

    if( NOT EXISTS "${CMAKE_SOURCE_DIR}/${pchHeaderFile}" )
        ebsLogGenInternalWarning( "PCH Header ${pchHeaderFile} for project ${pProjectID} not found, skipping PCH setup." )
        return()
    endif()

    set( pchBinaryOutputDir "${PROJECT_BINARY_DIR}/${pProjectID}.dir" )
    get_filename_component( pchBaseName ${pchHeaderName} NAME_WE )
    ebsVmsSysconfGet( "Host.Compiler" compiler )

    if( ${compiler} STREQUAL "MSVC" )

        if( NOT EXISTS "${CMAKE_SOURCE_DIR}/${pchSourceFile}" )
            ebsLogPrintUtilCommonAlwaysOn( "PCH Source ${pchSourceFile} for project ${pProjectID} not found, skipping PCH setup." )
            return()
        endif()

        set( pchBinary "${pchBinaryOutputDir}/${pchBaseName}.pch" ) 

        set_source_files_properties( ${pchSourceFile} PROPERTIES
            COMPILE_FLAGS "/Yc\"${pchIncName}\" /Fp\"${pchBinary}\"" OBJECT_OUTPUTS "${pchBinary}" )
        set_source_files_properties( ${${pSourceFileList}} PROPERTIES
            COMPILE_FLAGS "/Yu\"${pchIncName}\" /FI\"${pchIncName}\" /Fp\"${pchBinary}\"" OBJECT_DEPENDS "${pchBinary}" )

        source_group( "_Precomp" FILES ${pchSourceFile} )
        set( ${outPchFileName} ${pchSourceFile} PARENT_SCOPE )
        
    elseif( ${compiler} STREQUAL "Clang" )
        
        set( pchBinary "${pchBinaryOutputDir}/${pchBaseName}.h.pth" )

        set_source_files_properties( ${pchHeaderFile} PROPERTIES
            COMPILE_FLAGS "-emit-pth -o ${pchBinary}" OBJECT_OUTPUTS "${pchBinary}" )
        set_source_files_properties( ${${pSourceFileList}} PROPERTIES
            COMPILE_FLAGS "-include-pth ${pchBinary}" OBJECT_DEPENDS "${pchBinary}" )

        source_group( "" FILES ${pchHeaderFile} )
        set( ${outPchFileName} ${pchHeaderFile} PARENT_SCOPE )
        
    elseif( ${compiler} STREQUAL "GCC" )
        
        set( pchBinary "${pchBinaryOutputDir}/${pchBaseName}.h.gch" )

        set_source_files_properties( ${pchHeaderFile} PROPERTIES
            COMPILE_FLAGS "-x" OBJECT_OUTPUTS "${pchBinary}" )
        set_source_files_properties( ${${pSourceFileList}} PROPERTIES
            OBJECT_DEPENDS "${pchBinary}" )

        source_group( "" FILES ${pchHeaderFile} )
        set( ${outPchFileName} ${pchHeaderFile} PARENT_SCOPE )

    endif()

endfunction()
