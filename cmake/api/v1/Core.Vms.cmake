
set( _EBS_VMSDEF_VARCATEGORYLIST
    "Registry"
    "Sysconf"
    "Workspace" )

############################################################################
############################################################################
############################################################################

set( _EBS_VMSDEF_VARLIST_Registry_ItemList_INTERNAL
    #[[ItemList]] #Public
    #[[ItemList]] #Internal
    #[[ItemList]] "Global" # List of all items in the registry (groups and projects)
    #[[ItemList]] "Group" # List of registsred groups (ids)
    #[[ItemList]] "Project" # List of registsred projects (ids)
)

set( _EBS_VMSDEF_VARLIST_Registry_ItemProperty_INTERNAL
    #[[ItemProperty]] #Public
    #[[ItemProperty]] #Internal
    #[[ItemProperty]] "Name"
    #[[ItemProperty]] "ParentGroupID"
    #[[ItemProperty]] "ParentGroupName"
)

set( _EBS_VMSDEF_VARLIST_Registry_ItemPropertyGroup_INTERNAL
    #[[ItemProperty]] #Public
    #[[ItemProperty]] #Internal
    #[[ItemProperty]] "SubGroupList"
)

set( _EBS_VMSDEF_VARDEF_Registry_ItemList_Global TRUE )
set( _EBS_VMSDEF_VARDEF_Registry_ItemList_Group TRUE )
set( _EBS_VMSDEF_VARDEF_Registry_ItemList_Project TRUE )

set( _EBS_VMSDEF_VARDEF_Registry_ItemProperty_Name TRUE )
set( _EBS_VMSDEF_VARDEF_Registry_ItemProperty_ParentGroupID TRUE )
set( _EBS_VMSDEF_VARDEF_Registry_ItemProperty_ParentGroupName TRUE )

set( _EBS_VMSDEF_VARDEF_Registry_ItemPropertyGroup_SubGroupList TRUE )

############################################################################
############################################################################
############################################################################

set( _EBS_VMSDEF_VARLIST_Sysconf_Config
    #[[Config]] #Public
    #[[Config]] #Internal
    #[[Config]] "InitStatus"
    #[[Config]] "WorkspaceRootDir"
)

set( _EBS_VMSDEF_VARLIST_Sysconf_Host
    #[[Host]] #Public
    #[[Host]] #Internal
    #[[Host]] "Compiler"
    #[[Host]] "CompilerVersion"
    #[[Host]] "FlagGenMultitarget"
    #[[Host]] "SystemName"
    #[[Host]] "SystemVersion"
    #[[Host]] "SharedLibExtension"
    #[[Host]] "SharedLibNamePrefix"
    #[[Host]] "StaticLibExtension"
    #[[Host]] "StaticLibNamePrefix"
    #[[Host]] "ToolsetArch"
    #[[Host]] "ToolsetArchBit"
)

set( _EBS_VMSDEF_VARLIST_Sysconf_Target
    #[[Target]] #Public
    #[[Target]] "Arch"
    #[[Target]] "ArchBit"
    #[[Target]] "BuildConfig"
    #[[Target]] "CppVersion"
    #[[Target]] "CppVersionEnableExt"
    #[[Target]] "ExtendedInstructionSet"
    #[[Target]] "FlagEnableDebugInfo"
    #[[Target]] "System"
    #[[Target]] "SysAPIOverride"
    #[[Target]] #Internal
    #[[Target]] "ArchDefault32"
    #[[Target]] "ArchDefault64"
    #[[Target]] "ArchDefaultBit"
    #[[Target]] "FlagArchARMFamily"
    #[[Target]] "FlagArchX86Family"
    #[[Target]] "FlagSystemAndroid"
    #[[Target]] "FlagSystemApple"
    #[[Target]] "FlagSystemPOSIX"
    #[[Target]] "FlagOSWinFamily"
    #[[Target]] "Platform"
    #[[Target]] "SysAPI"
)

set( _EBS_VMSDEF_VARLIST_Sysconf_Xbuild
    #[[Build]] #Public
    #[[Build]] #Internal
    #[[Build]] "CompileDefs"
    #[[Build]] "CompileFlags"
)

set( _EBS_VMSDEF_VARLIST_Sysconf_Xprop
    #[[Xprop]] #Public
    #[[Xprop]] "AndroidAPI"
    #[[Xprop]] "AndroidAPIMinReq"
    #[[Xprop]] "AndroidNDKPath"
    #[[Xprop]] "AndroidSDKPath"
    #[[Xprop]] "WinPhoneVersion"
    #[[Xprop]] "WinRTVersion"
    #[[Xprop]] #Public
)

set( _EBS_VMSDEF_VARDEF_Sysconf_Config_InitStatus TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Config_WorkspaceRootDir TRUE "VTypeInternal" )

set( _EBS_VMSDEF_VARDEF_Sysconf_Host_Compiler TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_CompilerVersion TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_FlagGenMultitarget TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_OSName TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_OSVersion TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_SharedLibExtension TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_SharedLibNamePrefix TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_StaticLibExtension TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_StaticLibNamePrefix TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_ToolsetArch TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Host_ToolsetArchBit TRUE "VTypeInternal" )

set( _EBS_VMSDEF_VARDEF_Sysconf_Target_Arch TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_ArchBit TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_BuildConfig TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_CppVersion TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_CppVersionEnableExt TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_ExtendedInstructionSet TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_FlagEnableDebugInfo TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_OS TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_SysAPIOverride TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_ArchDefault32 TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_ArchDefault64 TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_ArchDefaultBit TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_FlagArchARMFamily TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_FlagArchX86Family TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_FlagOSAndroid TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_FlagOSApple TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_FlagOSPOSIX TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_FlagOSWinFamily TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_Platform TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Target_SysAPI TRUE "VTypeInternal" )

set( _EBS_VMSDEF_VARDEF_Sysconf_Xbuild_CompileDefs TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Sysconf_Xbuild_CompileFlags TRUE "VTypeInternal" )

set( _EBS_VMSDEF_VARDEF_Sysconf_Xprop_AndroidAPI TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Xprop_AndroidAPIMinReq TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Xprop_AndroidNDKPath TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Xprop_AndroidSDKPath TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Xprop_WinPhoneVersion TRUE )
set( _EBS_VMSDEF_VARDEF_Sysconf_Xprop_WinRTVersion TRUE )

############################################################################
############################################################################
############################################################################

set( _EBS_VMSDEF_VARLIST_Workspace_Config
    #[[Config]] #Public
    #[[Config]] "DefaultCompilerCCVersion"
    #[[Config]] "DefaultCompilerCXXVersion"
    #[[Config]] "OutputSubdirExecutable"
    #[[Config]] "OutputSubdirLibShared"
    #[[Config]] "OutputSubdirLibStatic"
    #[[Config]] #Internal
)

set( _EBS_VMSDEF_VARLIST_Workspace_ItmCommonProperty
    #[[ItmCommonProperty]] #Public
    #[[ItmCommonProperty]] "ArtifactType"
    #[[ItmCommonProperty]] "BinPath"
    #[[ItmCommonProperty]] "CompileDefs"
    #[[ItmCommonProperty]] "CompileFlags"
    #[[ItmCommonProperty]] "IncludePath"
    #[[ItmCommonProperty]] "LinkFlags"
    #[[ItmCommonProperty]] "LinkInput"
    #[[ItmCommonProperty]] "LinkSearchPath"
    #[[ItmCommonProperty]] "PchActive"
    #[[ItmCommonProperty]] "PchFileHeader"
    #[[ItmCommonProperty]] "PchFileSource"
    #[[ItmCommonProperty]] "RuntimeLibDependencies"
    #[[ItmCommonProperty]] "SourceFileTypeMask"
    #[[ItmCommonProperty]] "TargetCompilerCCVersion"
    #[[ItmCommonProperty]] "TargetCompilerCXXVersion"
    #[[ItmCommonProperty]] "TargetCompilerEnableEIS"
    #[[ItmCommonProperty]] "TargetCompilerEnableObjC"
    #[[ItmCommonProperty]] "TargetCompilerForceEIS"
    #[[ItmCommonProperty]] "TargetCompilerEnableExtensions"
    #[[ItmCommonProperty]] "TargetCompilerVersionRequire"
    #[[ItmCommonProperty]] #Internal
)

set( _EBS_VMSDEF_VARLIST_Workspace_ItmGlobalProperty
    #[[ItmGlobalProperty]] #Public
)

set( _EBS_VMSDEF_VARLIST_Workspace_ItmGroupProperty
    #[[ItmGroupProperty]] #Public
    #[[ItmGroupProperty]] "BaseDir" # Base directory for all projects in a group, relative to workspace root.
    #[[ItmGroupProperty]] "DefaultRootDir" # Root dir for projects. Useful if all projects follow the same pattern, e.g. %PROJECT_NAME%.
    #[[ItmGroupProperty]] "DependencyWhitelist" # List of groups, whose projects are allowed as a dependencies.
    #[[ItmGroupProperty]] "IncludeSubdir"
    #[[ItmGroupProperty]] "OutputFilenamePrefix"
    #[[ItmGroupProperty]] "OutputFilenameSuffix"
    #[[ItmGroupProperty]] "ProjectNamePrefix" #
    #[[ItmGroupProperty]] "TargetOSSupportList" #
    #[[ItmGroupProperty]] #Internal
    #[[ItmGroupProperty]] "FullRootDir"
)

set( _EBS_VMSDEF_VARLIST_Workspace_ItmProjectProperty
    #[[ItmProjectProperty]] #Public
    #[[ItmProjectProperty]] "IncludeSubdir"
    #[[ItmProjectProperty]] "InternalDependencies"
    #[[ItmProjectProperty]] "RootDir" # Root dir for the project. Overwrites Group:DefaultRootDir.
    #[[ItmProjectProperty]] "SourceFileList"
    #[[ItmProjectProperty]] "SourceFileListNoFilter"
    #[[ItmProjectProperty]] "SourceFileMask"
    #[[ItmProjectProperty]] "SourceFileMaskNoFilter"
    #[[ItmProjectProperty]] "TargetOSSupportList" #
    #[[ItmProjectProperty]] #Internal
    #[[ItmProjectProperty]] "FullRootDir"
    #[[ItmProjectProperty]] "ResultOutputDir"
)

set( _EBS_VMSDEF_VARDEF_Workspace_Config_DefaultCompilerCCVersion TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_Config_DefaultCompilerCXXVersion TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_Config_OutputSubdirExecutable TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_Config_OutputSubdirLibShared TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_Config_OutputSubdirLibStatic TRUE )

set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_ArtifactType TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_BinPath TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_CompileDefs TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_CompileFlags TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_IncludePath TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_LinkFlags TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_LinkInput TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_LinkSearchPath TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_PchActive TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_PchFileHeader TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_PchFileSource TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_RuntimeLibDependencies TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_SourceFileTypeMask TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_TargetCompilerCCVersion TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_TargetCompilerCXXVersion TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_TargetCompilerEnableEIS TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_TargetCompilerEnableObjC TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_TargetCompilerForceEIS TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_TargetCompilerEnableExtensions TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_TargetCompilerVersionRequire TRUE )

set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_BaseDir TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_DefaultRootDir TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_DependencyWhitelist TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_IncludeSubdir TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_OutputFilenamePrefix TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_OutputFilenameSuffix TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_ProjectNamePrefix TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_TargetOSSupportList TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_FullRootDir TRUE "VTypeInternal" )

set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_IncludeSubdir TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_InternalDependencies TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_RootDir TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_SourceFileList TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_SourceFileListNoFilter TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_SourceFileMask TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_SourceFileMaskNoFilter TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_TargetOSSupportList TRUE )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_FullRootDir TRUE "VTypeInternal" )
set( _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_ResultOutputDir TRUE "VTypeInternal" )

set( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmCommonProperty_CompileDefs TRUE )
set( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmCommonProperty_CompileFlags TRUE )
set( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmCommonProperty_IncludePath TRUE )
set( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmCommonProperty_LinkFlags TRUE )
set( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmCommonProperty_LinkInput TRUE )
set( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmCommonProperty_LinkSearchPath TRUE )


set( _EBS_VMSDEF_VARVALUELIST_Sysconf_Host_Compiler
    "Clang"
    "GCC"
    "ICC"
    "MSVC"
)

set( _EBS_VMSDEF_VARVALUELIST_Sysconf_Target_Arch
    "ARM"
    "ARM64"
    "x86"
    "x86_64"
)

set( _EBS_VMSDEF_VARVALUELIST_Sysconf_Target_ExtendedInstructionSet
    "SSE"
    "SSE2"
    "SSE3"
    "SSE3X"
    "SSE4A"
    "SSE41"
    "SSE42"
    "AVX"
    "AVX2"
    "NEON"
)

set( _EBS_VMSDEF_VARVALUELIST_Workspace_ItmGroupProperty_ArtifactType
    "Executable"
    "LibShared"
    "LibStatic"
)

set( _EBS_VMSDEF_VARVALUELIST_Workspace_ItmProjectProperty_ArtifactType
    "Executable"
    "LibShared"
    "LibStatic"
)

set( _EBS_VMSINTERNAL_SYSCONF_EIS_FORCE_COMPILE_DEF_STR
    "__EBS_EIS_FORCE__"
    )

set( _EBS_VMSINTERNAL_SYSCONF_EIS_LEVEL_COMPILE_DEF_MAP
    "NEONv1=__EBS_EIS_LEVEL_NEONv1__"
    "NEONv2=__EBS_EIS_LEVEL_NEONv2__"
    "SSE=__EBS_EIS_LEVEL_SSE__"
    "SSE2=__EBS_EIS_LEVEL_SSE2__"
    "SSE3=__EBS_EIS_LEVEL_SSE3__"
    "SSE3X=__EBS_EIS_LEVEL_SSE3X__"
    "SSE4A=__EBS_EIS_LEVEL_SSE4A__"
    "SSE41=__EBS_EIS_LEVEL_SSE41__"
    "SSE42=__EBS_EIS_LEVEL_SSE42__"
    "AVX=__EBS_EIS_LEVEL_AVX__"
    "AVX2=__EBS_EIS_LEVEL_AVX2__"
    )

set( _EBS_VMSINTERNAL_SYSCONF_EIS_LEVEL_COMPILE_OPTION_MAP_Clang
    "SSE=-msse"
    "SSE2=-msse2"
    "SSE3=-msse3"
    "SSE3X=-mssse3"
    "SSE4A=-msse4a"
    "SSE41=-msse4.1"
    "SSE42=-msse4.2"
    "AVX=-mavx"
    "AVX2=-mavx2"
    )

set( _EBS_VMSINTERNAL_SYSCONF_EIS_LEVEL_COMPILE_OPTION_MAP_GCC
    "SSE=-msse"
    "SSE2=-msse2"
    "SSE3=-msse3"
    "SSE3X=-mssse3"
    "SSE4A=-msse4a"
    "SSE41=-msse4.1"
    "SSE42=-msse4.2"
    "AVX=-mavx"
    "AVX2=-mavx2"
    )

set( _EBS_VMSINTERNAL_SYSCONF_EIS_LEVEL_COMPILE_OPTION_MAP_ICC
    "NEONv1="
    "NEONv2="
    "SSE=/arch:SSE"
    "SSE2=/arch:SSE2"
    "SSE3=/arch:SSE3"
    "SSE3X=/arch:SSSE3"
    "SSE4A=/arch:SSE4"
    "SSE41=/arch:SSE4.1"
    "SSE42=/arch:SSE4.2"
    "AVX=/arch:AVX"
    "AVX2=/arch:CORE-AVX2"
    )

set( _EBS_VMSINTERNAL_SYSCONF_EIS_LEVEL_COMPILE_OPTION_MAP_MSVC
    "SSE=/arch:SSE"
    "SSE2=/arch:SSE2"
    "SSE3=/arch:SSE2"
    "SSE3X=/arch:SSE2"
    "SSE4A=/arch:SSE2"
    "SSE41=/arch:SSE2"
    "SSE42=/arch:SSE2"
    "AVX=/arch:AVX"
    "AVX2=/arch:AVX2"
    )


# Config variables:
# - config category: EBS_CONFIG_ENABLE_LOG_CATEGORY_VMS_CORE
# - var trace: EBS_CONFIG_ENABLE_VMS_VARTRACE

#
macro( ebsLogVmsCoreDbginfo )
    ebsLogDbginfo( "VMS_CORE" ${ARGN} )
endmacro()
#
macro( ebsLogVmsCoreError )
    ebsLogError( "VMS_CORE" ${ARGN} )
endmacro()

#
function( _ebsVmsGetItmGlobalPropertyName pVarCategory pVarGroup pVarBaseName outItmGlobalPropertyName )
    set( ${outItmGlobalPropertyName} "${pVarCategory}_${pVarGroup}_${pVarBaseName}" PARENT_SCOPE )
endfunction()

#
function( _ebsVmsGetVarSystemName pVarCategory pVarGroup pVarBaseName pVmsItemID outVarSystemName )
    if( pVmsItemID )
        set( pVmsItemID "_${pVmsItemID}" )
    endif()
    set( ${outVarSystemName} "EBSVAR_${pVarCategory}_${pVarGroup}${pVmsItemID}_${pVarBaseName}" PARENT_SCOPE )
endfunction()

#
function( _ebsVmsParseVarRefName pVarRefName outVarCategory outItmGroupProperty outVarBaseName )

    ebsCommonUtilStringSplit( ${pVarRefName} "." varNameComponentList )

    list( LENGTH varNameComponentList listLength )

    if( ${listLength} EQUAL 3 )
        list( GET varNameComponentList 0 varCategory )
        list( GET varNameComponentList 1 varGroup )
        list( GET varNameComponentList 2 varBaseName )
    elseif( ${listLength} EQUAL 2 )
        list( GET varNameComponentList 0 varGroup )
        list( GET varNameComponentList 1 varBaseName )
    elseif( ${listLength} EQUAL 1 )
        list( GET varNameComponentList 0 varBaseName )
    endif()

    if( varCategory )
        set( ${outVarCategory} ${varCategory} PARENT_SCOPE )
    endif()

    if( varGroup )
        set( ${outItmGroupProperty} ${varGroup} PARENT_SCOPE )
    endif()

    if( varBaseName )
        set( ${outVarBaseName} ${varBaseName} PARENT_SCOPE )
    endif()

endfunction()


function( _ebsVmsCheckAccessImpl pInternalMode pVarCategory pVarGroup pVarBaseName pValue )

    # Check if specified VMS category is valid (it should be in the category list).
    if( NOT ${pVarCategory} IN_LIST _EBS_VMSDEF_VARCATEGORYLIST )
        ebsLogVmsCoreError( "Unknown category: ${pVarCategory}" )
    endif()

    _ebsVmsGetItmGlobalPropertyName( ${pVarCategory} ${pVarGroup} ${pVarBaseName} globalVarName )

    #
    if( NOT ${pVarBaseName} IN_LIST _EBS_VMSDEF_VARLIST_${pVarCategory}_${pVarGroup} )
        if( NOT ${pVarBaseName} IN_LIST _EBS_VMSDEF_VARLIST_${pVarCategory}_${pVarGroup}_INTERNAL )
            ebsLogVmsCoreError( "${globalVarName}: unknown variable" )
        endif()
    endif()

    #
    if( NOT _EBS_VMSDEF_VARDEF_${globalVarName} )
        ebsLogVmsCoreError( "${globalVarName}: missing inline definition" )
    endif()

    #
    if( NOT pInternalMode AND ( "VTypeInternal" IN_LIST _EBS_VMSDEF_VARDEF_${pVarCategory}_${pVarGroup}_${pVarBaseName} ) )
        ebsLogVmsCoreError( "${globalVarName}: access denied" )
    endif()

    if( pValue )
        set( valueListName _EBS_VMSDEF_VARVALUELIST_${globalVarName} )
        if( ${valueListName} )
            if( NOT ${pValue} IN_LIST ${valueListName} )
                ebsLogVmsCoreError( "Invalid value '${pValue}' for variable ${globalVarName}" )
            endif()
        endif()
    endif()

endfunction()


#
function( _ebsVmsGetImpl pVarRefName pVmsItemID outValue )

    _ebsVmsParseVarRefName( ${pVarRefName} varCategory varGroup varBaseName )
    _ebsVmsCheckAccessImpl( TRUE ${varCategory} ${varGroup} ${varBaseName} "" )
    _ebsVmsGetVarSystemName( ${varCategory} ${varGroup} ${varBaseName} "${pVmsItemID}" systemVarName )

    if( EBS_CONFIG_ENABLE_VMS_VARTRACE )
        if( ${systemVarName} )
            ebsLogVmsCoreDbginfo( "GET ${systemVarName} <-- ${${systemVarName}}" )
        else()
            ebsLogVmsCoreDbginfo( "GET ${systemVarName} (var not set)" )
        endif()
    endif()

    set( ${outValue} "${${systemVarName}}" PARENT_SCOPE )

endfunction()


#
function( _ebsVmsFetchImpl pVarRefName pVmsItemID outValue outIsSet )

    _ebsVmsParseVarRefName( ${pVarRefName} varCategory varGroup varBaseName )
    _ebsVmsCheckAccessImpl( TRUE ${varCategory} ${varGroup} ${varBaseName} "" )
    _ebsVmsGetVarSystemName( ${varCategory} ${varGroup} ${varBaseName} "${pVmsItemID}" systemVarName )

    if( DEFINED ${systemVarName} )
        set( isSet TRUE )
    endif()

    if( EBS_CONFIG_ENABLE_VMS_VARTRACE )
        if( ${systemVarName} )
            ebsLogVmsCoreDbginfo( "FTC ${systemVarName} <-- ${${systemVarName}}" )
        elseif( DEFINED ${systemVarName} )
            ebsLogVmsCoreDbginfo( "FTC ${systemVarName} (empty)" )
        else()
            ebsLogVmsCoreDbginfo( "FTC ${systemVarName} (var not set)" )
        endif()
    endif()

    set( ${outValue} "${${systemVarName}}" PARENT_SCOPE )
    set( ${outIsSet} ${isSet} PARENT_SCOPE )

endfunction()


#
function( _ebsVmsSetImpl pInternalMode pVarRefName pVmsItemID pValue )

    _ebsVmsParseVarRefName( ${pVarRefName} varCategory varGroup varBaseName )
    _ebsVmsCheckAccessImpl( "${pInternalMode}" ${varCategory} ${varGroup} ${varBaseName} "" )
    _ebsVmsGetVarSystemName( ${varCategory} ${varGroup} ${varBaseName} "${pVmsItemID}" systemVarName )

    if( EBS_CONFIG_ENABLE_VMS_VARTRACE )
        if( ${systemVarName} )
            ebsLogVmsCoreDbginfo( "SET ${systemVarName} --> ${pValue} (current: ${${systemVarName}})" )
        else()
            ebsLogVmsCoreDbginfo( "SET ${systemVarName} --> ${pValue} (new)" )
        endif()
    endif()

    set( ${systemVarName} "${pValue}" CACHE INTERNAL "${systemVarName}" FORCE )

    # Add the variable to the registry.
    _ebsRegistryAddEntry( ${varCategory} ${systemVarName} )

endfunction()


#
function( _ebsVmsAppendImpl pInternalMode pVarRefName pVmsItemID pValue )

    _ebsVmsParseVarRefName( ${pVarRefName} varCategory varGroup varBaseName )
    _ebsVmsCheckAccessImpl( "${pInternalMode}" ${varCategory} ${varGroup} ${varBaseName} "" )
    _ebsVmsGetVarSystemName( ${varCategory} ${varGroup} ${varBaseName} "${pVmsItemID}" systemVarName )

    if( EBS_CONFIG_ENABLE_VMS_VARTRACE )
        if( ${systemVarName} )
            ebsLogVmsCoreDbginfo( "ADD ${systemVarName} (${${systemVarName}}) +== ${pValue}" )
        else()
            ebsLogVmsCoreDbginfo( "ADD ${systemVarName} (not found) +== ${pValue}" )
        endif()
    endif()

    ebsCommonUtilListAppend( ${systemVarName} "${pValue}" XUNIQUE XSORT )
    set( ${systemVarName} ${${systemVarName}} CACHE INTERNAL "${systemVarName}" FORCE )

    # Add the variable to the registry.
    _ebsRegistryAddEntry( ${varCategory} ${systemVarName} )

endfunction()


#
function( _ebsVmsClearImpl pInternalMode pVarRefName pVmsItemID )

    _ebsVmsParseVarRefName( ${pVarRefName} varCategory varGroup varBaseName )
    _ebsVmsCheckAccessImpl( "${pInternalMode}" ${varCategory} ${varGroup} ${varBaseName} "" )
    _ebsVmsGetVarSystemName( ${varCategory} ${varGroup} ${varBaseName} "${pVmsItemID}" systemVarName )

    if( EBS_CONFIG_ENABLE_VMS_VARTRACE )
        if( ${systemVarName} )
            ebsLogVmsCoreDbginfo( "CLR ${systemVarName} (current: ${${systemVarName}})" )
        elseif( DEFINED ${systemVarName} )
            ebsLogVmsCoreDbginfo( "CLR ${systemVarName} (emopty)" )
        else()
            ebsLogVmsCoreDbginfo( "CLR ${systemVarName} (not set!)" )
        endif()
    endif()

    set( ${systemVarName} "" CACHE INTERNAL "${systemVarName}" FORCE )

endfunction()


#
function( _ebsVmsUnsetImpl pInternalMode pVarRefName pVmsItemID )

    _ebsVmsParseVarRefName( ${pVarRefName} varCategory varGroup varBaseName )
    _ebsVmsCheckAccessImpl( "${pInternalMode}" ${varCategory} ${varGroup} ${varBaseName} "" )
    _ebsVmsGetVarSystemName( ${varCategory} ${varGroup} ${varBaseName} "${pVmsItemID}" systemVarName )

    if( EBS_CONFIG_ENABLE_VMS_VARTRACE )
        if( ${systemVarName} )
            ebsLogVmsCoreDbginfo( "RMV ${systemVarName} (current: ${${systemVarName}})" )
        elseif( DEFINED ${systemVarName} )
            ebsLogVmsCoreDbginfo( "RMV ${systemVarName} (emopty)" )
        else()
            ebsLogVmsCoreDbginfo( "RMV ${systemVarName} (not set!)" )
        endif()
    endif()

    unset( ${systemVarName} CACHE )

    # Add the variable to the registry.
    _ebsRegistryRemoveEntry( ${varCategory} ${systemVarName} )

endfunction()

#
macro( _ebsVmsSetInternal pVarRefName pVmsItemID pValue )
    _ebsVmsSetImpl( TRUE ${pVarRefName} "${pVmsItemID}" "${pValue}" )
endmacro()
#
macro( _ebsVmsAppendInternal pVarRefName pVmsItemID pValue )
    _ebsVmsAppendImpl( TRUE ${pVarRefName} "${pVmsItemID}" "${pValue}" )
endmacro()
#
macro( _ebsVmsUnsetInternal pVarRefName pVmsItemID )
    _ebsVmsUnsetImpl( TRUE ${pVarRefName} "${pVmsItemID}" )
endmacro()
#
macro( _ebsVmsClearInternal pVarRefName pVmsItemID )
    _ebsVmsClearImpl( TRUE ${pVarRefName} "${pVmsItemID}" )
endmacro()

#
macro( ebsVmsGet pVarRefName pVmsItemID outValue )
    _ebsVmsGetImpl( ${pVarRefName} "${pVmsItemID}" ${outValue} )
endmacro()
#
macro( ebsVmsFetch pVarRefName pVmsItemID outValue outIsSet )
    _ebsVmsFetchImpl( ${pVarRefName} "${pVmsItemID}" ${outValue} ${outIsSet} )
endmacro()
#
macro( ebsVmsSet pVarRefName pVmsItemID pValue )
    _ebsVmsSetImpl( FALSE ${pVarRefName} "${pVmsItemID}" "${pValue}" )
endmacro()
#
macro( ebsVmsAppend pVarRefName pVmsItemID pValue )
    _ebsVmsAppendImpl( FALSE ${pVarRefName} "${pVmsItemID}" "${pValue}" )
endmacro()
#
macro( ebsVmsUnset pVarRefName pVmsItemID )
    _ebsVmsUnsetImpl( FALSE ${pVarRefName} "${pVmsItemID}" )
endmacro()
#
macro( ebsVmsClear pVarRefName pVmsItemID )
    _ebsVmsClearImpl( FALSE ${pVarRefName} "${pVmsItemID}" )
endmacro()
