
#
macro( ebsLogGenInternalDbginfo )
    ebsLogDbginfo( "GENERATOR" ${ARGN} )
endmacro()
#
macro( ebsLogGenInternalInfo )
    ebsLogInfo( "GENERATOR" ${ARGN} )
endmacro()
#
macro( ebsLogGenInternalWarning )
    ebsLogWarning( "GENERATOR" ${ARGN} )
endmacro()
#
macro( ebsLogGenInternalError )
    ebsLogError( "GENERATOR" ${ARGN} )
endmacro()

#
function( _ebsGenUpdateTargetBuildConfig pProjectID )

    ebsVmsSysconfGet( "Host.Compiler" compiler )
    ebsVmsSysconfGet( "Target.System" targetSystem )
    ebsVmsSysconfGet( "Target.ExtendedInstructionSet" eisLevel )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "TargetCompilerEnableEIS" enableEIS )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "TargetCompilerForceEIS" forceEIS )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "TargetCompilerEnableObjC" enableObjC )

    if( enableEIS AND _EBS_VMSINTERNAL_SYSCONF_EIS_LEVEL_COMPILE_OPTION_MAP_${compiler} )
        foreach( eisOption ${_EBS_VMSINTERNAL_SYSCONF_EIS_LEVEL_COMPILE_OPTION_MAP_${compiler}} )
            if( "${eisOption}" MATCHES "${eisLevel}=" )
                set( eisOptionStr "${eisOption}" )
                string( REPLACE "${eisLevel}=" "" eisOptionStr "${eisOptionStr}" )
                break()
            endif()
        endforeach()

        foreach( eisDef ${_EBS_VMSINTERNAL_SYSCONF_EIS_LEVEL_COMPILE_DEF_MAP} )
            if( "${eisDef}" MATCHES "${eisLevel}=" )
                set( eisDefStr "${eisDef}" )
                string( REPLACE "${eisLevel}=" "" eisDefStr "${eisDefStr}" )
                break()
            endif()
        endforeach()

        ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileFlags" ${eisOptionStr} )
        ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileDefs" "${eisDefStr}=1" )

        if( forceEIS )
            set( eisForceDefStr "${_EBS_VMSINTERNAL_SYSCONF_EIS_FORCE_COMPILE_DEF_STR}" )
            ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileDefs" "${eisForceDefStr}=1" )
        endif()
    endif()

    ebsVmsSysconfGet( "Target.BuildConfig" buildConfig )
    ebsVmsSysconfGet( "Target.FlagEnableDebugInfo" enableDebugInfo )

    if( "${buildConfig}" STREQUAL "Release" )
        ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileDefs" "NDEBUG=1" )
        ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileDefs" "DEBUG=0" )
        ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileDefs" "_DEBUG=0" )
    else()
        ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileDefs" "DEBUG=1" )
        ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileDefs" "_DEBUG=1" )
    endif()

    if( enableObjC AND ( "${targetSystem}" MATCHES "iOS|macOS" ) )
        # ebsVmsWorkspaceAppend( "Project:${pProjectID}" "CompileFlags" "-x objective-c++" )
    endif()

endfunction()

#
function( _ebsGenCheckTargetCompilerVersion pProjectID )

    ebsVmsWorkspaceQuery( "Config" "DefaultCompilerCCVersion" defaultCCVersion )
    ebsVmsWorkspaceQuery( "Config" "DefaultCompilerCXXVersion" defaultCXXVersion )

    if( NOT defaultCCVersion )
        set( defaultCCVersion 90 )
    endif()

    if( NOT defaultCXXVersion )
        set( defaultCXXVersion 14 )
    endif()

    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "TargetCompilerCCVersion" ccVersion )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "TargetCompilerCXXVersion" cxxVersion )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "TargetCompilerEnableExtensions" enableExt )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "TargetCompilerVersionRequire" versionRequire )

    if( ccVersion )
        if( NOT "${ccVersion}" MATCHES "90|99|11" )
            set( ccVersion ${defaultCCVersion} )
        endif()
        set( CMAKE_C_STANDARD ${ccVersion} PARENT_SCOPE )
        set( CMAKE_C_EXTENSIONS "${enableExt}" PARENT_SCOPE )
        set( CMAKE_C_STANDARD_REQUIRED "${versionRequire}" PARENT_SCOPE )
    else() #[[( cxxVersion )]]
        if( NOT "${cxxVersion}" MATCHES "98|11|14|17" )
            set( cxxVersion ${defaultCXXVersion} )
        endif()
        set( CMAKE_CXX_STANDARD ${cxxVersion} PARENT_SCOPE )
        set( CMAKE_CXX_EXTENSIONS "${enableExt}" PARENT_SCOPE )
        set( CMAKE_CXX_STANDARD_REQUIRED "${versionRequire}" PARENT_SCOPE )
    endif()

endfunction()

#
function( ebsUtilCommonGetFileListFromMask pBasePath pMaskList outFileList )

    file( GLOB fullBasePath "${pBasePath}" )

    foreach( fileMask ${pMaskList} )
        file( GLOB tmpFileList "${pBasePath}/${fileMask}" )
        list( APPEND fileList "${tmpFileList}" )
    endforeach()

    foreach( file ${fileList} )
        string( REPLACE "${fullBasePath}/" "" file ${file} )
        list( APPEND resultList ${file} )
    endforeach()

    set( ${outFileList} "${resultList}" PARENT_SCOPE )

endfunction()

#
function( ebsUtilCommonGetSourceFileGroupStr pBasePath pFilePath outResult )

    get_filename_component( resultGroup "${pFilePath}" DIRECTORY )

    string( REPLACE "${pBasePath}" "" resultGroup "${resultGroup}" )

    string( REPLACE "." "\\" resultGroup "${resultGroup}" )
    string( REPLACE "\\" "\\\\" resultGroup "${resultGroup}" )

    set( ${outResult} ${resultGroup} PARENT_SCOPE )

endfunction()

function( _ebsGenBaseGetProjectSourceFilesInternal pProjectID pProjectPath pSrcFmaskVarName pSrcFileVarName pSrcGroupName outFileList )

    if( pSrcFmaskVarName )
        ebsVmsWorkspaceGet( "Project:${pProjectID}" "${pSrcFmaskVarName}" sourceFileMaskList )
        if( sourceFileMaskList )
            ebsUtilCommonGetFileListFromMask( ${pProjectPath} "${sourceFileMaskList}" sourceFileList )
        endif()
    else()
        ebsVmsWorkspaceGet( "Project:${pProjectID}" "${pSrcFileVarName}" sourceFileList )
    endif()

    if( sourceFileList )
        foreach( srcFile ${sourceFileList} )
            if( pSrcGroupName )
                set( srcGroupName ${pSrcGroupName} )
            else()
                ebsUtilCommonGetSourceFileGroupStr( ${pProjectPath} ${srcFile} srcGroupName )
            endif()
            source_group( "${srcGroupName}" FILES "${pProjectPath}/${srcFile}" )
            list( APPEND resultSourceFileList "${pProjectPath}/${srcFile}" )
        endforeach()
    endif()

    set( ${outFileList} "${resultSourceFileList}" PARENT_SCOPE )

endfunction()


#
function( ebsGenBaseGetProjectSourceFiles pProjectID pProjectPath outSourceFileList )

    _ebsGenBaseGetProjectSourceFilesInternal( ${pProjectID} ${pProjectPath} "" "SourceFileList" "" sourceFileList )
    list( APPEND projectSourceFileList "${sourceFileList}" )
    unset( sourceFileList )

    _ebsGenBaseGetProjectSourceFilesInternal( ${pProjectID} ${pProjectPath} "" "SourceFileListNoFilter" "*" sourceFileList )
    list( APPEND projectSourceFileList "${sourceFileList}" )
    unset( sourceFileList )

    _ebsGenBaseGetProjectSourceFilesInternal( ${pProjectID} ${pProjectPath} "SourceFileMask" "" "" sourceFileList )
    list( APPEND projectSourceFileList "${sourceFileList}" )
    unset( sourceFileList )

    _ebsGenBaseGetProjectSourceFilesInternal( ${pProjectID} ${pProjectPath} "SourceFileMaskNoFilter" "" "*" sourceFileList )
    list( APPEND projectSourceFileList "${sourceFileList}" )
    unset( sourceFileList )

    ebsGenSpecificAddPrecompiledHeader( ${pProjectID} ${pProjectPath} "${projectSourceFileList}" pchFileName )
    list( APPEND projectSourceFileList "${pchFileName}" )
    unset( pchFileName )

    set( ${outSourceFileList} "${projectSourceFileList}" PARENT_SCOPE )

endfunction()


#
function( ebsGenBaseAddProjectTarget pGoupID pProjectID pSourceFileList pDbgListFiles outTargetStatus )

    if( NOT pSourceFileList )
        ebsLogGenInternalWarning( "No source files has been defined for project ${pProjectID}. Target ignored." )
        return()
    endif()

    if( pDbgListFiles )
        ebsLogGenInternalDbginfo( "> Source files:" )
        foreach( file ${pSourceFileList} )
            ebsLogGenInternalDbginfo( "> -- ${file}" )
        endforeach()
    endif()

    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "ArtifactType" projectArtifactType )

    if( NOT projectArtifactType )
        set( projectArtifactType "Executable" )
    endif()

    _ebsGenCheckTargetCompilerVersion( ${pProjectID} )
    _ebsGenUpdateTargetBuildConfig( ${pProjectID} )

	#
	if( ${projectArtifactType} STREQUAL "Executable" )
        add_executable( ${pProjectID} ${pSourceFileList} )
    elseif( ${projectArtifactType} STREQUAL "LibShared" )
        add_library( ${pProjectID} SHARED ${pSourceFileList} )
    elseif( ${projectArtifactType} STREQUAL "LibStatic" )
        add_library( ${pProjectID} STATIC ${pSourceFileList} )
    else()
        ebsLogPrintGenWarning( "Target ${project} has no artifact defined. Target ignored." )
        return()
    endif()

    set( ${outTargetStatus} TRUE PARENT_SCOPE )

endfunction()


#
function( ebsGenBaseSetupProjectProperties pGroupID pProjectID pProjectBaseName )

    ebsVmsWorkspaceGet( "Group:${pGroupID}" "FullRootDir" outputRootDir )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "ArtifactType" projectArtifactType )
    ebsVmsWorkspaceQuery( "Config" "OutputSubdir${projectArtifactType}" outputSubdir )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "OutputFilenamePrefix" outputFilenamePrefix )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "OutputFilenameSuffix" outputFilenameSuffix )

	set( targetOutputFile "${outputFilenamePrefix}${pProjectBaseName}${outputFilenameSuffix}" )
    set( targetOutputDirectory "${outputRootDir}/${outputSubdir}" )

	# Set output directories.
	set_target_properties( ${pProjectID} PROPERTIES
		OUTPUT_NAME "${targetOutputFile}"
		ARCHIVE_OUTPUT_DIRECTORY "${targetOutputDirectory}"
		LIBRARY_OUTPUT_DIRECTORY "${targetOutputDirectory}"
		RUNTIME_OUTPUT_DIRECTORY "${targetOutputDirectory}" )

    _ebsVmsWorkspaceSetInternal( FALSE "Project:${pProjectID}" "ResultOutputDir" "${targetOutputDirectory}" )

    ebsVmsSysconfGet( "Xbuild.CompileDefs" sysconfCompileDefs )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "CompileDefs" targetCompileDefs )
    set( targetCompileDefs "${targetCompileDefs};${sysconfCompileDefs}" )
    ebsLogGenInternalInfo( "-- targetCompileDefs: ${targetCompileDefs}" )
    target_compile_definitions( ${pProjectID} PUBLIC ${targetCompileDefs} )

    ebsVmsSysconfGet( "Xbuild.CompileFlags" sysconfCompileFlags )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "CompileFlags" targetCompileFlags )
    set( targetCompileFlags "${targetCompileFlags};${sysconfCompileFlags}" )
    ebsLogGenInternalInfo( "-- targetCompileFlags: ${targetCompileFlags}" )
    target_compile_options( ${pProjectID} PUBLIC ${targetCompileFlags} )

    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "IncludePath" targetIncludePath )
    ebsLogGenInternalInfo( "-- targetIncludePath: ${targetIncludePath}" )
    target_include_directories( ${pProjectID} PUBLIC ${targetIncludePath} )

    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "LinkFlags" targetLinkOptions )
    ebsLogGenInternalInfo( "-- targetLinkOptions: ${targetLinkOptions}" )
    target_link_libraries( ${pProjectID} ${targetLinkOptions} )

    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "LinkInput" targetLinkInput )
    ebsLogGenInternalInfo( "-- targetLinkInput: ${targetLinkInput}" )
    target_link_libraries( ${pProjectID} ${targetLinkInput} )

    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "LinkSearchPath" linkSearchPath )
    ebsLogGenInternalInfo( "-- linkSearchPath: ${linkSearchPath}" )
    link_directories( ${pProjectID} ${linkSearchPath} )

    foreach( linkerPath ${linkSearchPath} )
        target_link_libraries( ${pProjectID} "-L${linkerPath}" )
    endforeach()

endfunction()


#
function( ebsGenBaseGetProjectDependencyList pCallStack pProjectID outDependencyList )
    ebsVmsWorkspaceGet( "Project:${pProjectID}" "InternalDependencies" projectDependencies )
    foreach( dependency ${projectDependencies} )
        if( NOT ${dependency} IN_LIST pCallStack )
            ebsCommonUtilListAppend( pCallStack ${dependency} )
            ebsGenBaseGetProjectDependencyList( "${pCallStack}" ${dependency} subdependencies )
            ebsCommonUtilListAppend( projectDependencies "${subdependencies}" XUNIQUE )
            ebsUtilCommonListRemove( pCallStack ${dependency} )
        endif()
    endforeach()
    set( ${outDependencyList} "${projectDependencies}" PARENT_SCOPE )
endfunction()


#
function( ebsGenBaseSetupProjectInternalDependencies pGroupID pProjectID )

    math( EXPR dependenciesNum 0 )
    ebsLogGenInternalInfo( "-- Setting internal dependencies for ${pProjectID}..." )

    ebsVmsWorkspaceGet( "Group:${pGroupID}" "DependencyWhitelist" dependencyWhitelist )
    ebsGenBaseGetProjectDependencyList( "${pProjectID}" ${pProjectID} projectDependencies )

    if( projectDependencies )
        # 'dependency' is an ID of a project which pProjectID depends on.
        foreach( dependency ${projectDependencies} )
            ebsWorkspaceGetProjectGroup( ${dependency} dependencyGroup )

            if( NOT "${dependencyGroup}" STREQUAL "${pGroupID}" )
                if( NOT ${dependencyGroup} IN_LIST dependencyWhitelist )
                    ebsLogGenInternalWarning( "-- Project ${dependency} [${dependencyGroup}] is not allowed as a dependency for ${pProjectID} [${pGroupID}]" )
                    continue()
                endif()
            endif()

            target_link_libraries( ${pProjectID} ${dependency} )

            ebsVmsWorkspaceGet( "Project:${dependency}" "FullRootDir" fullRootDir )
            ebsVmsWorkspaceQueryEx( "Project:${dependency}" "IncludeSubdir" includeSubdir isIncludeSubdirSet )

            if( isIncludeSubdirSet )
                get_filename_component( projectIncludePath "${fullRootDir}${includeSubdir}" ABSOLUTE )
            else()
                get_filename_component( projectIncludePath "${fullRootDir}" ABSOLUTE )
            endif()

            string( REPLACE "//" "." projectIncludePath ${projectIncludePath} )
            ebsCommonUtilListAppend( dependencyIncludePathList ${projectIncludePath} XUNIQUE )

            ebsLogGenInternalInfo( "---- ${pProjectID} --> ${dependency}" )
            math( EXPR dependenciesNum "${dependenciesNum}+1" )
        endforeach()

        if( dependencyIncludePathList )
            ebsVmsWorkspaceAppend( "Project:${pProjectID}" "IncludePath" "${dependencyIncludePathList}" )
        endif()
    endif()

    ebsLogGenInternalInfo( "---- Done (added: ${dependenciesNum})" )

endfunction()


#
function( ebsGenBaseSetupProjectBinaryDependencies pGroupID pProjectID )

    math( EXPR dependenciesNum 0 )
    ebsLogGenInternalInfo( "-- Setting runtime binaries for ${pProjectID}..." )

    ebsVmsWorkspaceGet( "Project:${pProjectID}" "RuntimeLibDependencies" runtimeLibDependencies )
    ebsVmsWorkspaceQuery( "Project:${pProjectID}" "BinPath" binPathList )

    if( runtimeLibDependencies )
        ebsVmsWorkspaceGet( "Project:${pProjectID}" "ResultOutputDir" resultOutputDir )
        ebsVmsSysconfGet( "Host.SharedLibExtension" sharedLibExtension )
        ebsVmsSysconfGet( "Host.SharedLibNamePrefix" sharedLibNamePrefix )
        # binDependency is
        foreach( runtimeLib ${runtimeLibDependencies} )
            foreach( binPath ${binPathList} )
                set( runtimeLibFileName "${binPath}/${sharedLibNamePrefix}${runtimeLib}${sharedLibExtension}" )
                if( EXISTS ${runtimeLibFileName} )
                    add_custom_command( TARGET ${pProjectID} PSystemT_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy ${runtimeLibFileName} ${resultOutputDir} )
                    ebsLogGenInternalInfo( "---- ${pProjectID} --> ${runtimeLib} (${runtimeLibFileName})" )
                    math( EXPR dependenciesNum "${dependenciesNum}+1" )
                    break()
                else()
                    ebsLogGenInternalInfo( "---- ${runtimeLib}: ${runtimeLibFileName} not found, skipped" )
                endif()
            endforeach()
        endforeach()
    endif()

    ebsLogGenInternalInfo( "---- Done (added: ${dependenciesNum})" )

endfunction()
