
#
function( _ebsGenCheckCompilerConfiguration )

    ebsVmsSysconfGet( "Target.CppVersion" targetCppVersion )

    if( NOT "${targetCppVersion}" MATCHES "11|14|17" )
        ebsVmsSysconfSet( "Target.CppVersion" 11 )
        set( targetCppVersion 11 )
    endif()

    ebsVmsSysconfGet( "Host.Compiler" compiler )
    ebsVmsSysconfGet( "Host.CompilerVersion" compilerVersion )

    if( "${compiler}" MATCHES "Clang" )
        set( cxxVersionFlag "-std=c++${targetCppVersion}" )
    elseif( "${compiler}" MATCHES "ICC" )
        set( cxxVersionFlag "/Qstd=c++${targetCppVersion}" )
    elseif( "${compiler}" MATCHES "MSVC" )
        if( ${compilerVersion} GREATER_EQUAL 2017 )
            set( cxxVersionFlag "/std:c++${targetCppVersion}" )
        endif()
    elseif( "${compiler}" MATCHES "GCC" )
        ebsVmsSysconfGet( "Target.CppVersionEnableExt" cxxExtGNU )
        if( cxxExtGNU )
            set( cxxVersionFlag "-std=gnu++${targetCppVersion}" )
        else()
            set( cxxVersionFlag "-std=c++${targetCppVersion}" )
        endif()
    endif()

    if( cxxVersionFlag )
        _ebsVmsSysconfAppendInternal( "Xbuild.CompileFlags" ${cxxVersionFlag} )
    endif()

endfunction()

#
function( _ebsGenCheckTargetEISConfiguration )

    set( ARMEISMatchStr "NEON" )
    set( X86EISMatchStr "SSE|SSE2|SSE3|SSE3X|SSE4A|SSE41|SSE42|AVX|AVX2" )

    ebsVmsSysconfGet( "Target.ExtendedInstructionSet" targetArch )
    ebsVmsSysconfGet( "Target.ExtendedInstructionSet" targetEIS )

    if( targetEIS )
        if( "${targetArch}" MATCHES "ARM" )

            if( NOT "${targetEIS}" MATCHES "${ARMEISMatchStr}" )
                ebsVmsSysconfUnset( "Target.ExtendedInstructionSet" )
            endif()

        elseif( "${targetArch}" MATCHES "x86" )

            if( NOT "${targetEIS}" MATCHES "${X86EISMatchStr}" )
                ebsVmsSysconfUnset( "Target.ExtendedInstructionSet" )
            endif()

        endif()
    endif()

endfunction()

#
function( ebsGenUpdateConfiguration )

    # _ebsGenCheckCompilerConfiguration()
    _ebsGenCheckTargetEISConfiguration()

endfunction()

#
function( ebsGenBuildWorkspaceTree )

    ebsWorkspaceRegGetGroupList( groupList )
    ebsWorkspaceRegGetProjectList( projectList )

    foreach( groupID ${groupList} )

        ebsWorkspacSetGroupPathConfiguration( ${groupID} )

    endforeach()

    foreach( projectID ${projectList} )

        ebsWorkspaceGetProjectGroup( ${projectID} groupID )

        ebsWorkspacSetProjectPathConfiguration( ${projectID} projectRootSubdir )

        ebsLogGenInternalInfo( "AddSubidrectory: ${projectRootSubdir}" )
        add_subdirectory( "${projectRootSubdir}" )

        ebsGenBaseGetProjectSourceFiles( ${projectID} ${projectRootSubdir} sourceFileList )
        ebsGenBaseAddProjectTarget( ${groupID} ${projectID} "${sourceFileList}" FALSE targetStatus )

        if( targetStatus )
            ebsCommonUtilListAppend( activeProjectList ${projectID} XUNIQUE )
        endif()

        unset( sourceFileList )
        unset( targetStatus )

    endforeach()

    foreach( projectID ${activeProjectList} )

        ebsLogGenInternalInfo( ">> Configuring project: ${projectID}" )

        ebsWorkspaceGetProjectGroup( ${projectID} groupID )
        ebsVmsGet( "Registry.ItemProperty.Name" "Project:${projectID}" projectName )

        ebsGenBaseSetupProjectInternalDependencies( ${groupID} ${projectID} )
        ebsGenBaseSetupProjectProperties( ${groupID} ${projectID} ${projectName} )
        ebsGenBaseSetupProjectBinaryDependencies( ${groupID} ${projectID} )

    endforeach()

endfunction()
