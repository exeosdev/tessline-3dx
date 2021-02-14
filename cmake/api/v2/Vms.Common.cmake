
# List of all variables that are part of the system. Build system's core is consisted
# of variables used to store configuration of the workspace and current/target system.
# There are three variable categories:

set( _EBS_VMSDEF_VARLIST_Internal
    #Sysconf.Core/Internal
    "Sysconf.Core.InitStatus"
	"Sysconf.Core.PlatformConfigString"
	"Sysconf.Core.WorkspaceRootDir"

    #Sysconf.Host/Internal
    "Sysconf.Host.Compiler"
    "Sysconf.Host.CompilerVersion"
    "Sysconf.Host.FlagGenMultitarget"
    "Sysconf.Host.SystemName"
    "Sysconf.Host.SystemVersion"
    "Sysconf.Host.SharedLibExtension"
    "Sysconf.Host.SharedLibNamePrefix"
    "Sysconf.Host.StaticLibExtension"
    "Sysconf.Host.StaticLibNamePrefix"
    "Sysconf.Host.ToolsetArch"
    "Sysconf.Host.ToolsetArchBit"

    #Sysconf.Target/Internal
    "Sysconf.Target.ArchDefault32"
    "Sysconf.Target.ArchDefault64"
    "Sysconf.Target.ArchDefaultBit"
    "Sysconf.Target.FlagArchARMFamily"
    "Sysconf.Target.FlagArchX86Family"
    "Sysconf.Target.FlagSystemAndroid"
    "Sysconf.Target.FlagSystemApple"
    "Sysconf.Target.FlagSystemPOSIX"
    "Sysconf.Target.FlagSystemWinFamily"
    "Sysconf.Target.Platform"
	"Sysconf.Target.SharedLibExtension"
	"Sysconf.Target.SharedLibNamePrefix"
	"Sysconf.Target.StaticLibExtension"
	"Sysconf.Target.StaticLibNamePrefix"
    "Sysconf.Target.SysAPI"

	#Workspace.CoreItemList/Internal
	"Workspace.CoreItemList.Global"
	"Workspace.CoreItemList.Group"
	"Workspace.CoreItemList.Project"
	"Workspace.CoreItemList.UIDMap"

	#Workspace.CoreItemProp/Internal
	"Workspace.CoreItemProp.ParentGroupID"
	"Workspace.CoreItemProp.SubGroupList"
	"Workspace.CoreItemProp.SubProjectList"

    #Workspace.PropGlobal/Internal

    #Workspace.PropGroup/Internal

    #Workspace.PropProject/Internal
)

set( _EBS_VMSDEF_VARLIST_Public
    #Sysconf.Target/Public
    "Sysconf.Target.Arch"
    "Sysconf.Target.ArchBit"
    "Sysconf.Target.Buildcfg"
    "Sysconf.Target.CppVersion"
    "Sysconf.Target.CppVersionEnableExt"
    "Sysconf.Target.ExtendedInstructionSet"
    "Sysconf.Target.FlagEnableDebugInfo"
    "Sysconf.Target.System"
    "Sysconf.Target.SysAPIOverride"

    #Sysconf.Xconf/Public
    "Sysconf.Xconf.AndroidAPI"
    "Sysconf.Xconf.AndroidAPIMinReq"
    "Sysconf.Xconf.AndroidNDKPath"
    "Sysconf.Xconf.AndroidSDKPath"

    #Workspace.Config/Public
    "Workspace.Config.DefaultCompilerCCVersion"
    "Workspace.Config.DefaultCompilerCXXVersion"
    "Workspace.Config.OutputSubdirExecutable"
    "Workspace.Config.OutputSubdirLibShared"
    "Workspace.Config.OutputSubdirLibStatic"

    #Workspace.PropCommon/Public
    "Workspace.PropCommon.ArtifactType"
    "Workspace.PropCommon.BinPath"
    "Workspace.PropCommon.CompileDefs"
    "Workspace.PropCommon.CompileFlags"
    "Workspace.PropCommon.IncludePath"
    "Workspace.PropCommon.LinkFlags"
    "Workspace.PropCommon.LinkInput"
    "Workspace.PropCommon.LinkSearchPath"
    "Workspace.PropCommon.PchActive"
    "Workspace.PropCommon.PchFileHeader"
    "Workspace.PropCommon.PchFileSource"
    "Workspace.PropCommon.RuntimeLibDependencies"
    "Workspace.PropCommon.SourceFileTypeMask"
    "Workspace.PropCommon.TargetCompilerCCVersion"
    "Workspace.PropCommon.TargetCompilerCXXVersion"
    "Workspace.PropCommon.TargetCompilerEnableEIS"
    "Workspace.PropCommon.TargetCompilerEnableObjC"
    "Workspace.PropCommon.TargetCompilerForceEIS"
    "Workspace.PropCommon.TargetCompilerEnableExtensions"
    "Workspace.PropCommon.TargetCompilerVersionRequire"
	
    #Workspace.PropGlobal/Public

    #Workspace.PropGroup/Public
    "Workspace.PropGroup.BaseDir" # Base directory for all projects in a group, relative to workspace root.
    "Workspace.PropGroup.DefaultRootDir" # Root dir for projects. Useful if all projects follow the same pattern, e.g. %PROJECT_NAME%_v2.
    "Workspace.PropGroup.DependencyWhitelist" # List of groups, whose projects are allowed as a dependencies.
    "Workspace.PropGroup.IncludeSubdir"
    "Workspace.PropGroup.OutputFilenamePrefix"
    "Workspace.PropGroup.OutputFilenameSuffix"
    "Workspace.PropGroup.ProjectNamePrefix" #
    "Workspace.PropGroup.TargetSystemSupportList" #

    #Workspace.PropProject/Public
    "Workspace.PropProject.IncludeSubdir"
    "Workspace.PropProject.InternalDependencies"
    "Workspace.PropProject.RootDir" # Root dir for the project. Overwrites Group:DefaultRootDir.
    "Workspace.PropProject.SourceFileList"
    "Workspace.PropProject.SourceFileListNoFilter"
    "Workspace.PropProject.SourceFileMask"
    "Workspace.PropProject.SourceFileMaskNoFilter"
    "Workspace.PropProject.TargetSystemSupportList" #
)


macro( ebsVmsLogAlwaysOn pMessage )
	ebsCoreLog( "Vms" "AlwaysOn" "${pMessage}" )
endmacro()

macro( ebsVmsLogError pMessage )
	ebsCoreLog( "Vms" "Error" "${pMessage}" )
endmacro()

macro( ebsVmsLogWarning pMessage )
	ebsCoreLog( "Vms" "Warning" "${pMessage}" )
endmacro()

macro( ebsVmsLogInfo pMessage )
	ebsCoreLog( "Vms" "Info" "${pMessage}" )
endmacro()

macro( ebsVmsLogDebug pMessage )
	ebsCoreLog( "Vms" "Debug" "${pMessage}" )
endmacro()

#@FunDef _ebsVmsParseVarRefName
#@DocStr Parses "reference name" of a variable, i.e. name used by the user to reference a variable. The format is
#@DocStr defined as Category.Group.Name. It splits the name and stores each component in the corresponding variable.
function( _ebsVmsParseVarRefName pVarRefName outCategory outGroup outVarName )
	# Split the var name, in format <Category.Group.Name>
	ebsCoreUtilStringSplit( ${pVarRefName} "." nameComponentsList )
	# There should be three name components
	list( LENGTH nameComponentsList listLength )
	if( NOT ( ${listLength} EQUAL 3 ) )
		return()
	endif()
	# Get values and store them in corresponding variables
	list( GET nameComponentsList 0 category )
	set( ${outCategory} ${category} PARENT_SCOPE )
	list( GET nameComponentsList 1 group )
	set( ${outGroup} ${group} PARENT_SCOPE )
	list( GET nameComponentsList 2 baseName )
	set( ${outVarName} ${baseName} PARENT_SCOPE )
endfunction()

#@FunDef _ebsVmsCheckAccess
#@DocStr
function( _ebsVmsCheckAccess pInternalMode pVarCategory pVarGroup pVarBaseName )
	# We need the full reference name here, too.
	set( varRefName "${pVarCategory}.${pVarGroup}.${pVarBaseName}" )

	if( NOT ${varRefName} IN_LIST _EBS_VMSDEF_VARLIST_Public )
		if( NOT ${varRefName} IN_LIST _EBS_VMSDEF_VARLIST_Internal )
			set( varUnknown TRUE )
		else()
			set( varInternal TRUE )
		endif()
	endif()

	if( varUnknown )
		ebsVmsLogError( "Unknown variable: ${varRefName}" )
	endif()
	if( varInternal AND NOT pInternalMode )
		ebsVmsLogError( "Access denied: ${varRefName} is INTERNAL" )
	endif()
endfunction()

#@FunDef _ebsVmsGetVarSystemName
#@DocStr
function( _ebsVmsGetVarSystemName pVarCategory pVarGroup pVarBaseName pWorkspaceItemID outSystemName )
	ebsCoreUtilStringReplace( "${pWorkspaceItemID}" "." "_" pWorkspaceItemID )
	if( pWorkspaceItemID )
		set( ${outSystemName} "_EBSVAR_${pVarCategory}_[${pWorkspaceItemID}]_${pVarGroup}_${pVarBaseName}" PARENT_SCOPE )
	else()
		set( ${outSystemName} "_EBSVAR_${pVarCategory}_${pVarGroup}_${pVarBaseName}" PARENT_SCOPE )
	endif()
endfunction()

#@FunDef _ebsVmsPrepareCommand
#@DocStr
function( _ebsVmsPrepareCommand pCommand pInternalMode pVarRefName pWorkspaceItemID pValue outSystemName outIsSet )
	# Parse ref name and get the category, group and base name.
	_ebsVmsParseVarRefName( ${pVarRefName} varCategory varGroup varBaseName )
	# If at least one of these is not set, the name is invalid.
	if( NOT ( varCategory AND varGroup AND varBaseName ) )
		ebsVmsLogError( "Invalid variable name: ${pVarRefName}" )
		return()
	endif()
	# Check the access. For example, internal variables require ${pInternalMode} to be TRUE.
	_ebsVmsCheckAccess( "${pInternalMode}" ${varCategory} ${varGroup} ${varBaseName} )
	# Retrieve fully-qualified variable name, use to store its value in the system.
	# This is the name variable can be directly accessed with.
	_ebsVmsGetVarSystemName( ${varCategory} ${varGroup} ${varBaseName} "${pWorkspaceItemID}" varSystemName )

	if( DEFINED ${varSystemName} )
		set( varIsSet TRUE )
	endif()

	if( EBS_CONFIG_ENABLE_VMS_VARTRACE )
		if( "${pCommand}" STREQUAL "GET" )
			if( ${varSystemName} )
				ebsVmsLogDebug( "GET ${varSystemName}: ${${varSystemName}}" )
			else()
				ebsVmsLogDebug( "GET ${varSystemName}: NOTFOUND" )
			endif()
		elseif( "${pCommand}" STREQUAL "FETCH" )
			if( ${varSystemName} )
				ebsVmsLogDebug( "FETCH ${varSystemName}: ${${varSystemName}}" )
			elseif( varIsSet )
				ebsVmsLogDebug( "FETCH ${varSystemName}: EMPTY" )
			else()
				ebsVmsLogDebug( "FETCH ${varSystemName}: NOTFOUND" )
			endif()
		elseif( "${pCommand}" STREQUAL "ISSET" )
			if( ${varSystemName} )
				ebsVmsLogDebug( "ISSET ${varSystemName}: SET (VALUE)" )
			elseif( varIsSet )
				ebsVmsLogDebug( "ISSET ${varSystemName}: SET (EMPTY)" )
			else()
				ebsVmsLogDebug( "ISSET ${varSystemName}: NOTFOUND" )
			endif()
		elseif( "${pCommand}" STREQUAL "SET" )
			if( ${varSystemName} )
				ebsVmsLogDebug( "SET ${varSystemName}: ${pValue} (CURRENT: ${${varSystemName}})" )
			else()
				ebsVmsLogDebug( "SET ${varSystemName}: ${pValue} (NEW)" )
			endif()
		elseif( "${pCommand}" STREQUAL "APPEND" )
			if( ${varSystemName} )
				ebsVmsLogDebug( "APPEND ${varSystemName}: ${pValue} (CURRENT: ${${varSystemName}})" )
			else()
				ebsVmsLogDebug( "APPEND ${varSystemName}: ${pValue} (NEW)" )
			endif()
		elseif( "${pCommand}" STREQUAL "CLEAR" )
			if( ${varSystemName} )
				ebsVmsLogDebug( "CLEAR ${varSystemName} (CURRENT: ${${varSystemName}})" )
			elseif( varIsSet )
				ebsVmsLogDebug( "CLEAR ${varSystemName} (EMPTY)" )
			else()
				ebsVmsLogDebug( "CLEAR ${varSystemName} (NOTFOUND)" )
			endif()
		elseif( "${pCommand}" STREQUAL "UNSET" )
			if( ${varSystemName} )
				ebsVmsLogDebug( "UNSET ${varSystemName} (CURRENT: ${${varSystemName}})" )
			elseif( varIsSet )
				ebsVmsLogDebug( "UNSET ${varSystemName} (EMPTY)" )
			else()
				ebsVmsLogDebug( "UNSET ${varSystemName} (NOTFOUND)" )
			endif()
		endif()
	endif()

	if( outSystemName )
		set( ${outSystemName} "${varSystemName}" PARENT_SCOPE )
	endif()

	if( outIsSet )
		set( ${outIsSet} "${varIsSet}" PARENT_SCOPE )
	endif()

	# Add the variable to the registry.
	_ebsRegistryAddEntry( ${varCategory} ${varSystemName} )

endfunction()



macro( _ebsVmsRawSet pCmakeRawVarName )
	set( ${pCmakeRawVarName} "${ARGN}" CACHE INTERNAL "${pCmakeVariableName}" )
	# Add the variable to the registry.
	_ebsRegistryAddEntry( "" ${pCmakeRawVarName} )
endmacro()


#
function( _ebsVmsCmdGet pVarRefName pWorkspaceItemID outValue )
	#
	_ebsVmsPrepareCommand( "GET" TRUE ${pVarRefName} "${pWorkspaceItemID}" "" varSystemName varIsSet )
	#
	set( ${outValue} "${${varSystemName}}" PARENT_SCOPE )
endfunction()

#
function( _ebsVmsCmdFetch pVarRefName pWorkspaceItemID outValue outIsSet )
	#
	_ebsVmsPrepareCommand( "FETCH" TRUE ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" varSystemName varIsSet )
	#
	set( ${outValue} "${${varSystemName}}" PARENT_SCOPE )
	#
	set( ${outIsSet} "${varIsSet}" PARENT_SCOPE )
endfunction()

#
function( _ebsVmsCmdIsSet pVarRefName pWorkspaceItemID outIsSet )
	#
	_ebsVmsPrepareCommand( "ISSET" TRUE ${pVarRefName} "${pWorkspaceItemID}" "" "" varIsSet )
	#
	set( ${outIsSet} "${varIsSet}" PARENT_SCOPE )
endfunction()

#
function( _ebsVmsCmdSet pInternalMode pVarRefName pWorkspaceItemID pValue )
	#
	_ebsVmsPrepareCommand( "SET" "${pInternalMode}" ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" varSystemName varIsSet )
	#
	set( ${varSystemName} "${pValue}" CACHE INTERNAL "${varSystemName}" FORCE )
endfunction()

#
function( _ebsVmsCmdAppend pInternalMode pVarRefName pWorkspaceItemID pValue )
	#
	_ebsVmsPrepareCommand( "APPEND" "${pInternalMode}" ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" varSystemName varIsSet )
	#
	ebsCoreUtilListAppend( ${varSystemName} "${pValue}" XUNIQUE XSORT )
	#
	set( ${varSystemName} "${${varSystemName}}" CACHE INTERNAL "${varSystemName}" FORCE )
endfunction()

#
function( _ebsVmsCmdClear pInternalMode pVarRefName pWorkspaceItemID )
	#
	_ebsVmsPrepareCommand( "CLEAR" "${pInternalMode}" ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" varSystemName varIsSet )
	#
	set( ${varSystemName} "" CACHE INTERNAL "${varSystemName}" FORCE )
endfunction()

#
function( _ebsVmsCmdUnset pInternalMode pVarRefName pWorkspaceItemID )
	#
	_ebsVmsPrepareCommand( "UNSET" "${pInternalMode}" ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" varSystemName varIsSet )
	#
	unset( ${varSystemName} CACHE )
endfunction()


macro( _ebsVmsCmdSetInternal pVarRefName pWorkspaceItemID pValue )
	_ebsVmsCmdSet( TRUE ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" )
endmacro()

macro( _ebsVmsCmdAppendInternal pVarRefName pWorkspaceItemID pValue )
	_ebsVmsCmdAppend( TRUE ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" )
endmacro()

macro( _ebsVmsCmdClearInternal pVarRefName pWorkspaceItemID )
	_ebsVmsCmdClear( TRUE ${pVarRefName} "${pWorkspaceItemID}" )
endmacro()

macro( _ebsVmsCmdUnsetInternal pVarRefName pWorkspaceItemID )
	_ebsVmsCmdUnset( TRUE ${pVarRefName} "${pWorkspaceItemID}" )
endmacro()


macro( ebsVmsCmdGet pVarRefName pWorkspaceItemID outValue )
	_ebsVmsCmdGet( ${pVarRefName} "${pWorkspaceItemID}" ${outValue} )
endmacro()

macro( ebsVmsCmdFetch pVarRefName pWorkspaceItemID outValue outIsSet )
	_ebsVmsCmdFetch( ${pVarRefName} "${pWorkspaceItemID}" ${outValue} ${outIsSet} )
endmacro()

macro( ebsVmsCmdIsSet pVarRefName pWorkspaceItemID outIsSet )
	_ebsVmsCmdIsSet( ${pVarRefName} "${pWorkspaceItemID}" ${outIsSet} )
endmacro()

macro( ebsVmsCmdSet pVarRefName pWorkspaceItemID pValue )
	_ebsVmsCmdSet( FALSE ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" )
endmacro()

macro( ebsVmsCmdAppend pVarRefName pWorkspaceItemID pValue )
	_ebsVmsCmdAppend( FALSE ${pVarRefName} "${pWorkspaceItemID}" "${pValue}" )
endmacro()

macro( ebsVmsCmdClear pVarRefName pWorkspaceItemID )
	_ebsVmsCmdClear( FALSE ${pVarRefName} "${pWorkspaceItemID}" )
endmacro()

macro( ebsVmsCmdUnset pVarRefName pWorkspaceItemID )
	_ebsVmsCmdUnset( FALSE ${pVarRefName} "${pWorkspaceItemID}" )
endmacro()
