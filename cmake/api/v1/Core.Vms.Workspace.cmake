

# Config variables:
# - config category: EBS_CONFIG_ENABLE_LOG_CATEGORY_VMS_CORE

#
macro( ebsLogWorkspaceDbginfo )
    ebsLogDbginfo( "WORKSPACE" ${ARGN} )
endmacro()
#
macro( ebsLogWorkspaceInfo )
    ebsLogInfo( "WORKSPACE" ${ARGN} )
endmacro()
#
macro( ebsLogWorkspaceWarning )
    ebsLogWarning( "WORKSPACE" ${ARGN} )
endmacro()
#
macro( ebsLogWorkspaceError )
    ebsLogError( "WORKSPACE" ${ARGN} )
endmacro()

#
function( _ebsVmsWorkspaceCheckFilterMatch outMatch )
    ebsSysconfQueryCurrentPlatformFilterValues( compiler os sysAPI arch buildcfg )
    _ebsCoreFltParseFilterVarargInput( filterStringList "${ARGN}" )
    _ebsCoreFltCheckFilterMatch( "${filterStringList}" ${compiler} ${os} ${sysAPI} ${arch} ${buildcfg} checkResult )
    set( ${outMatch} ${checkResult} PARENT_SCOPE )
endfunction()

#
function( _ebsVmsWorkspaceIsItemRegistered pItemType pItemID outIsRegistered )

    ebsVmsGet( "Registry.ItemList.${pItemType}" "" registryList )

    if( ${pItemID} IN_LIST registryList )
        set( ${outIsRegistered} TRUE PARENT_SCOPE )
    endif()

endfunction()

#
function( _ebsVmsWorkspaceCheckRegStatus pItemType pItemID outRegStatus )

    if( "${pItemType}" MATCHES "Group|Project" )
        if( NOT pItemID )
            ebsLogWorkspaceError( "CheckRegStatus: missing item ID" )
            return()
        endif()
        _ebsVmsWorkspaceIsItemRegistered( ${pItemType} ${pItemID} isRegistered )
    else()
        set( isRegistered TRUE )
    endif()

    if( isRegistered )
        set( ${outRegStatus} TRUE PARENT_SCOPE )
    endif()

endfunction()

#
function( _ebsVmsWorkspaceCheckPropertyInheritMode pPropertyName outInheritMode )

    if( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_ItmCommonProperty )
        if( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmCommonProperty_${pPropertyName} )
            set( inheritMode TRUE )
        endif()
        #
    elseif( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_ItmGlobalProperty )
        if( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmGlobalProperty_${pPropertyName} )
            set( inheritMode TRUE )
        endif()
        #
    elseif( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_ItmGroupProperty )
        if( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmGroupProperty_${pPropertyName} )
            set( inheritMode TRUE )
        endif()
        #
    elseif( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_ItmProjectProperty )
        if( _EBS_VMSDEF_VARPROP_INHERIT_Workspace_ItmProjectProperty_${pPropertyName} )
            set( inheritMode TRUE )
        endif()
    endif()

    if( inheritMode )
        set( ${outInheritMode} TRUE PARENT_SCOPE )
    endif()
endfunction()

#
function( _ebsVmsWorkspaceGetPropertyVarRefName pItemType pPropertyName outVarRefName )
    if( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_Config )
        set( varRefName "Config.${pPropertyName}" )
    #
    elseif( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_ItmCommonProperty )
        set( varRefName "ItmCommonProperty.${pPropertyName}" )
    #
    elseif( ( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_ItmGlobalProperty ) AND ( ${pItemType} STREQUAL "Global" ) )
        set( varRefName "ItmGlobalProperty.${pPropertyName}" )
    #
    elseif( ( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_ItmGroupProperty ) AND ( ${pItemType} STREQUAL "Group" ) )
        set( varRefName "ItmGroupProperty.${pPropertyName}" )
    #
    elseif( ( "${pPropertyName}" IN_LIST _EBS_VMSDEF_VARLIST_Workspace_ItmProjectProperty ) AND ( ${pItemType} STREQUAL "Project" ) )
        set( varRefName "ItmProjectProperty.${pPropertyName}" )
    #
    else()
        ebsLogWorkspaceError( "InvalidPropertyRef: [${pItemType}.${pPropertyName}]" )
    endif()

    set( ${outVarRefName} ${varRefName} PARENT_SCOPE )
endfunction()

#
function( _ebsVmsWorkspaceParseItemRefName pItemRefName outItemType outItemID outVmsItemID )

    # - "Global" --> "Global" "" ""
    # - "Group:GID" --> "Group" "GID" "Group:GID"
    # - "Project:GID:PID" --> "Group" "GID" "Project:GID.PID"

    ebsCommonUtilStringSplit( ${pItemRefName} ":" itemRefNameComponentList )

    list( LENGTH itemRefNameComponentList listLength )

    if( ( ${listLength} EQUAL 0 ) OR ( ${listLength} GREATER 3 ) )
        ebsLogWorkspaceError( "Invalid ItemRefName: ${pItemRefName}" )
        return()
    endif()

    list( GET itemRefNameComponentList 0 itemType )

    if( NOT ( ${itemType} MATCHES "Config|Global|Group|Project" ) )
        ebsLogWorkspaceError( "Invalid Item type: ${itemType}" )
        return()
    endif()

    if( ${listLength} GREATER_EQUAL 2 )
        list( GET itemRefNameComponentList 1 itemID )
    endif()

    if( ${listLength} GREATER_EQUAL 3 )
        list( GET itemRefNameComponentList 2 itemIDStr2 )
        set( itemID "${itemID}:${itemIDStr2}" )
    endif()

    set( ${outItemType} ${itemType} PARENT_SCOPE )
    set( ${outItemID} "${itemID}" PARENT_SCOPE )
    set( ${outVmsItemID} "${pItemRefName}" PARENT_SCOPE )

endfunction()


function( _ebsVmsWorkspaceGetQueryItemRefNameList pItemType pItemID pPropertyName outResultList )
    if( "${pItemType}" STREQUAL "Config" )
        set( ${outResultList} "Config" PARENT_SCOPE )
        return()
    endif()

    if( "${pItemType}" STREQUAL "Project" )
        ebsVmsGet( "Registry.ItemProperty.ParentGroupID" "Project:${pItemID}" groupID )
        set( projectID "${pItemID}" )
    endif()

    if( "${pItemType}" STREQUAL "Group" ) # while( groupID ): append( groupID ); groupID = getParentGroup( groupID )
        set( groupID "${pItemID}" )
    endif()

    if( projectID AND ( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_${pPropertyName} OR _EBS_VMSDEF_VARDEF_Workspace_ItmProjectProperty_${pPropertyName} ) )
        ebsCommonUtilListAppend( resultList "Project:${projectID}" )
    endif()

    if( groupID AND ( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_${pPropertyName} OR _EBS_VMSDEF_VARDEF_Workspace_ItmGroupProperty_${pPropertyName} ) )
        while( groupID )
            ebsCommonUtilListAppend( resultList "Group:${groupID}" )
            ebsVmsGet( "Registry.ItemProperty.ParentGroupID" "Group:${groupID}" groupID )
        endwhile()
    endif()

    if( _EBS_VMSDEF_VARDEF_Workspace_ItmCommonProperty_${pPropertyName} OR _EBS_VMSDEF_VARDEF_Workspace_ItmGlobalProperty_${pPropertyName} )
        ebsCommonUtilListAppend( resultList "Global" )
    endif()

    set( ${outResultList} "${resultList}" PARENT_SCOPE )
endfunction()


function( _ebsVmsWorkspaceQuery pCheckRegStatus pItemRefName pPropertyName outValue outIsSet )
    _ebsVmsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID vmsItemID )
    _ebsVmsWorkspaceGetQueryItemRefNameList( ${itemType} "${itemID}" ${pPropertyName} itemRefNameList )
    _ebsVmsWorkspaceCheckPropertyInheritMode( ${pPropertyName} inheritMode )
    foreach( itemRefName ${itemRefNameList} )
        _ebsVmsWorkspaceFetch( ${pCheckRegStatus} ${itemRefName} ${pPropertyName} value isSet )
        if( isSet )
            if( value )
                ebsCommonUtilListAppend( resultValue "${value}" )
            endif()
            if( NOT resultIsSet )
                set( resultIsSet TRUE )
            endif()
            if( NOT inheritMode )
                break()
            endif()
        endif()
    endforeach()
    _ebsVmsWorkspaceReplaceDynamicRefs( "${itemType}" "${itemID}" "${resultValue}" resultValue )
    set( ${outValue} "${resultValue}" PARENT_SCOPE )
    set( ${outIsSet} "${resultIsSet}" PARENT_SCOPE )
endfunction()

#
function( _ebsVmsWorkspaceGet pCheckRegStatus pItemRefName pPropertyName outValue )
    _ebsVmsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID vmsItemID )
    if( pCheckRegStatus )
        _ebsVmsWorkspaceCheckRegStatus( ${itemType} "${itemID}" isItemRegistered )
    endif()
    if( NOT pCheckRegStatus OR isItemRegistered )
        _ebsVmsWorkspaceGetPropertyVarRefName( ${itemType} ${pPropertyName} varRefName )
        _ebsVmsGetImpl( "Workspace.${varRefName}" "${vmsItemID}" value )
    endif()
    set( ${outValue} "${value}" PARENT_SCOPE )
endfunction()

function( _ebsVmsWorkspaceFetch pCheckRegStatus pItemRefName pPropertyName outValue outIsSet )
    _ebsVmsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID vmsItemID )
    if( pCheckRegStatus )
        _ebsVmsWorkspaceCheckRegStatus( ${itemType} "${itemID}" isItemRegistered )
    endif()
    if( NOT pCheckRegStatus OR isItemRegistered )
        _ebsVmsWorkspaceGetPropertyVarRefName( ${itemType} ${pPropertyName} varRefName )
        _ebsVmsFetchImpl( "Workspace.${varRefName}" "${vmsItemID}" value isSet )
    endif()
    set( ${outValue} "${value}" PARENT_SCOPE )
    set( ${outIsSet} "${isSet}" PARENT_SCOPE )
endfunction()

function( _ebsVmsWorkspaceSet pCheckRegStatus pInternalMode pItemRefName pPropertyName pValue )
    _ebsVmsWorkspaceCheckFilterMatch( filterMatch "${ARGN}" )
    if( filterMatch )
        _ebsVmsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID vmsItemID )
        if( pCheckRegStatus )
            _ebsVmsWorkspaceCheckRegStatus( ${itemType} "${itemID}" isItemRegistered )
        endif()
        if( NOT pCheckRegStatus OR isItemRegistered )
            _ebsVmsWorkspaceGetPropertyVarRefName( ${itemType} ${pPropertyName} varRefName )
            _ebsVmsSetImpl( "${pInternalMode}" "Workspace.${varRefName}" "${vmsItemID}" "${pValue}" )
        endif()
    endif()
endfunction()

function( _ebsVmsWorkspaceAppend pCheckRegStatus pInternalMode pItemRefName pPropertyName pValue )
    _ebsVmsWorkspaceCheckFilterMatch( filterMatch "${ARGN}" )
    if( filterMatch )
        _ebsVmsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID vmsItemID )
        if( pCheckRegStatus )
            _ebsVmsWorkspaceCheckRegStatus( ${itemType} "${itemID}" isItemRegistered )
        endif()
        if( NOT pCheckRegStatus OR isItemRegistered )
            _ebsVmsWorkspaceGetPropertyVarRefName( ${itemType} ${pPropertyName} varRefName )
            _ebsVmsAppendImpl( "${pInternalMode}" "Workspace.${varRefName}" "${vmsItemID}" "${pValue}" )
        endif()
    endif()
endfunction()

function( _ebsVmsWorkspaceUnset pCheckRegStatus pInternalMode pItemRefName pPropertyName )
    _ebsVmsWorkspaceCheckFilterMatch( filterMatch "${ARGN}" )
    if( filterMatch )
        _ebsVmsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID vmsItemID )
        if( pCheckRegStatus )
            _ebsVmsWorkspaceCheckRegStatus( ${itemType} "${itemID}" isItemRegistered )
        endif()
        if( NOT pCheckRegStatus OR isItemRegistered )
            _ebsVmsWorkspaceGetPropertyVarRefName( ${itemType} ${pPropertyName} varRefName )
            _ebsVmsUnsetImpl( "${pInternalMode}" "Workspace.${varRefName}" "${vmsItemID}" )
        endif()
    endif()
endfunction()

function( _ebsVmsWorkspaceClear pCheckRegStatus pInternalMode pItemRefName pPropertyName )
    _ebsVmsWorkspaceCheckFilterMatch( filterMatch "${ARGN}" )
    if( filterMatch )
        _ebsVmsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID vmsItemID )
        if( pCheckRegStatus )
            _ebsVmsWorkspaceCheckRegStatus( ${itemType} "${itemID}" isItemRegistered )
        endif()
        if( NOT pCheckRegStatus OR isItemRegistered )
            _ebsVmsWorkspaceGetPropertyVarRefName( ${itemType} ${pPropertyName} varRefName )
            _ebsVmsClearImpl( "${pInternalMode}" "Workspace.${varRefName}" "${vmsItemID}" )
        endif()
    endif()
endfunction()


function( _ebsVmsWorkspaceReplaceDynamicRefs pItemType pItemID pInput outResult )

    if( "${pItemType}" STREQUAL "Project" )
        set( projectID ${pItemID} )
    endif()

    if( "${pItemType}" STREQUAL "Group" )
        set( groupID ${pItemID} )
    endif()

    if( projectID )
        ebsVmsGet( "Registry.ItemProperty.Name" "Project:${projectID}" projectName )
        ebsVmsGet( "Registry.ItemProperty.ParentGroupID" "Project:${projectID}" groupID )
        ebsVmsGet( "Workspace.ItmProjectProperty.FullRootDir" "Project:${projectID}" projectFullRootDir )
    endif()

    if( groupID )
        ebsVmsGet( "Registry.ItemProperty.Name" "Group:${groupID}" groupName )
        ebsVmsGet( "Workspace.ItmProjectProperty.FullRootDir" "Group:${groupID}" groupFullRootDir )
    endif()

    ebsVmsSysconfGet( "Config.WorkspaceRootDir" workspaceRootDir )
    ebsVmsSysconfGet( "Host.Compiler" compiler )
	ebsVmsSysconfGet( "Target.System" system )
	ebsVmsSysconfGet( "Target.Arch" arch )
    ebsVmsSysconfGet( "Target.BuildConfig" buildcfg )
    
    if( projectFullRootDir )
        string( REPLACE "%EBSV_PROJECT_DIR%" "${projectFullRootDir}" pInput "${pInput}" )
    endif()
    if( projectID )
        string( REPLACE "%EBSV_PROJECT_ID%" "${projectID}" pInput "${pInput}" )
    endif()
    if( projectName )
        string( REPLACE "%EBSV_PROJECT_NAME%" "${projectName}" pInput "${pInput}" )
    endif()
    if( groupFullRootDir )
        string( REPLACE "%EBSV_GROUP_DIR%" "${groupFullRootDir}" pInput "${pInput}" )
    endif()
    if( groupName )
        string( REPLACE "%EBSV_GROUP_NAME%" "${groupName}" pInput "${pInput}" )
    endif()
    if( workspaceRootDir )
        string( REPLACE "%EBSV_ROOT_DIR%" "${workspaceRootDir}" pInput "${pInput}" )
    endif()
    if( compiler )
        string( REPLACE "%EBSV_SYSCONF_COMPILER%" "${compiler}" pInput "${pInput}" )
    endif()
    if( os )
        string( REPLACE "%EBSV_SYSCONF_System%" "${os}" pInput "${pInput}" )
    endif()
    if( arch )
        string( REPLACE "%EBSV_SYSCONF_ARCH%" "${arch}" pInput "${pInput}" )
    endif()
    if( buildcfg )
        string( REPLACE "%EBSV_SYSCONF_BUILDCFG%" "${buildcfg}" pInput "${pInput}" )
    endif()

    set( ${outResult} "${pInput}" PARENT_SCOPE )

endfunction()



#
macro( _ebsVmsWorkspaceSetInternal pCheckRegStatus pItemRefName pPropertyName pValue )
    _ebsVmsWorkspaceSet( ${pCheckRegStatus} TRUE ${pItemRefName} ${pPropertyName} "${pValue}" )
endmacro()
#
macro( _ebsVmsWorkspaceAppendInternal pCheckRegStatus pItemRefName pPropertyName pValue )
    _ebsVmsWorkspaceAppend( ${pCheckRegStatus} TRUE ${pItemRefName} ${pPropertyName} "${pValue}" )
endmacro()
#
macro( _ebsVmsWorkspaceUnsetInternal pCheckRegStatus pItemRefName pPropertyName )
    _ebsVmsWorkspaceUnset( ${pCheckRegStatus} TRUE ${pItemRefName} ${pPropertyName} )
endmacro()
#
macro( _ebsVmsWorkspaceClearInternal pCheckRegStatus pItemRefName pPropertyName )
    _ebsVmsWorkspaceClear( ${pCheckRegStatus} TRUE ${pItemRefName} ${pPropertyName} )
endmacro()

#
macro( ebsVmsWorkspaceQueryEx pItemRefName pPropertyName outValue outIsSet )
    _ebsVmsWorkspaceQuery( FALSE ${pItemRefName} ${pPropertyName} ${outValue} ${outIsSet} )
endmacro()
#
macro( ebsVmsWorkspaceQuery pItemRefName pPropertyName outValue )
    _ebsVmsWorkspaceQuery( FALSE ${pItemRefName} ${pPropertyName} ${outValue} FALSE )
endmacro()
#
macro( ebsVmsWorkspaceGet pItemRefName pPropertyName outValue )
    _ebsVmsWorkspaceGet( FALSE ${pItemRefName} ${pPropertyName} ${outValue} )
endmacro()
#
macro( ebsVmsWorkspaceFetch pItemRefName pPropertyName outValue outIsSet )
    _ebsVmsWorkspaceFetch( FALSE ${pItemRefName} ${pPropertyName} ${outValue} ${outIsSet} )
endmacro()
#
macro( ebsVmsWorkspaceSet pItemRefName pPropertyName pValue )
    _ebsVmsWorkspaceSet( TRUE FALSE ${pItemRefName} ${pPropertyName} "${pValue}" "${ARGN}" )
endmacro()
#
macro( ebsVmsWorkspaceAppend pItemRefName pPropertyName pValue )
    _ebsVmsWorkspaceAppend( TRUE FALSE ${pItemRefName} ${pPropertyName} "${pValue}" "${ARGN}" )
endmacro()
#
macro( ebsVmsWorkspaceUnset pItemRefName pPropertyName )
    _ebsVmsWorkspaceUnset( TRUE FALSE ${pItemRefName} ${pPropertyName} "${ARGN}" )
endmacro()
#
macro( ebsVmsWorkspaceClear pItemRefName pPropertyName )
    _ebsVmsWorkspaceClear( TRUE FALSE ${pItemRefName} ${pPropertyName} "${ARGN}" )
endmacro()


function( ebsWorkspaceParseDynamicRefs pInput pItemRefName outResult )
    _ebsVmsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID vmsItemID )
    _ebsVmsWorkspaceReplaceDynamicRefs( "${itemType}" "${itemID}" ${pInput} result )
    set( ${outResult} "${result}" PARENT_SCOPE )
endfunction()

#
#
macro( ebsWorkspaceRegGetGroupList outGroupList )
    ebsVmsGet( "Registry.ItemList.Group" "" ${outGroupList} )
endmacro()

#
#
macro( ebsWorkspaceRegGetProjectList outProjectList )
    ebsVmsGet( "Registry.ItemList.Project" "" ${outProjectList} )
endmacro()

#
#
macro( ebsWorkspaceRegIsGroupRegistered pGroupID outIsRegistered )
    _ebsVmsWorkspaceCheckRegStatus( "Group" "${pGroupID}" ${outIsRegistered} )
endmacro()

#
#
macro( ebsWorkspaceRegIsProjectRegistered pProjectID outIsRegistered )
    _ebsVmsWorkspaceCheckRegStatus( "Project" "${pProjectID}" ${outIsRegistered} )
endmacro()

#
#
function( ebsWorkspaceRegAddGroup pParentGroupID pGroupName outGroupID )

	if( pParentGroupID )
        ebsWorkspaceRegIsGroupRegistered( ${pParentGroupID} isParentRegistered )
		if( NOT isParentRegistered )
            ebsLogWorkspaceError( "Group ${pParentGroupID} (parent for ${pGroupName}) has NOT been registered!" )
			return()
		endif()
        ebsVmsGet( "Registry.ItemProperty.Name" "Group:${pParentGroupID}" parentGroupName )
        set( groupID "${pParentGroupID}.${pGroupName}" )
    else()
        set( groupID "${pGroupName}" )
	endif()

    ebsWorkspaceRegIsGroupRegistered( ${groupID} isRegistered )
    if( isRegistered )
        ebsLogWorkspaceError( "Group ${groupID} has been already registered!" )
        return()
    endif()

    _ebsVmsAppendInternal( "Registry.ItemList.Group" "" ${groupID} )
	_ebsVmsAppendInternal( "Registry.ItemList.Global" "" "Group:${groupID}" )

    _ebsVmsSetInternal( "Registry.ItemProperty.Name" "Group:${groupID}" "${pGroupName}" )

    if( pParentGroupID )
        _ebsVmsAppendInternal( "Registry.ItemPropertyGroup.SubGroupList" "Group:${pParentGroupID}" "Group:${groupID}" )
        _ebsVmsSetInternal( "Registry.ItemProperty.ParentGroupID" "Group:${groupID}" "${pParentGroupID}" )
        _ebsVmsSetInternal( "Registry.ItemProperty.ParentGroupName" "Group:${groupID}" "${parentGroupName}" )
    endif()

    set( ${outGroupID} ${groupID} PARENT_SCOPE )

    ebsLogWorkspaceDbginfo( "<Group> GID:Parent: ${pParentGroupID} / GID = ${groupID}" )

endfunction()

#
#
function( ebsWorkspaceRegAddProject pParentGroupID pProjectName outProjectID )

    if( NOT pParentGroupID )
        ebsLogWorkspaceError( "Group ${pGroupName} has not been registered!" )
        return()
    endif()

    ebsWorkspaceRegIsGroupRegistered( ${pParentGroupID} isParentRegistered )
    if( NOT isParentRegistered )
        ebsLogWorkspaceError( "Group ${pGroupName} has not been registered!" )
        return()
    endif()

    set( projectID "${pParentGroupID}.${pProjectName}" )
    
    _ebsVmsWorkspaceCheckRegStatus( "Project" "${projectID}" isProjectRegistered )
    if( isProjectRegistered )
        ebsLogWorkspaceError( "[Warning] Project ${projectID} has been already registered!" )
        return()
    endif()

    _ebsVmsAppendInternal( "Registry.ItemList.Project" "" ${projectID} )
    _ebsVmsAppendInternal( "Registry.ItemList.Global" "" "Project:${projectID}" )

    ebsVmsGet( "Registry.ItemProperty.Name" "Group:${pParentGroupID}" parentGroupName )

    _ebsVmsSetInternal( "Registry.ItemProperty.Name" "Project:${projectID}" "${pProjectName}" )
    _ebsVmsSetInternal( "Registry.ItemProperty.ParentGroupID" "Project:${projectID}" "${pParentGroupID}" )
    _ebsVmsSetInternal( "Registry.ItemProperty.ParentGroupName" "Project:${projectID}" "${parentGroupName}" )

    set( ${outProjectID} ${projectID} PARENT_SCOPE )

    ebsLogWorkspaceDbginfo( "<Project> GID:Parent: ${pParentGroupID} / PID = ${projectID}" )

endfunction()

#
#
function( ebsWorkspaceGetProjectGroup pProjectID outGroupID )
    ebsVmsGet( "Registry.ItemProperty.ParentGroupID" "Project:${projectID}" groupID )
    set( ${outGroupID} "${groupID}" PARENT_SCOPE )
endfunction()

#
function( ebsWorkspacSetGroupPathConfiguration pGroupID )

	ebsVmsWorkspaceQueryEx( "Group:${pGroupID}" "BaseDir" groupBaseDir isBaseDirSet )

	set( groupRootSubdir "" )

	if( isBaseDirSet AND groupBaseDir )
		set( groupRootSubdir "${groupBaseDir}" )
	endif()

	string( REPLACE "//" "." groupRootSubdir "${groupRootSubdir}" )
    set( groupFullRootDir "${CMAKE_SOURCE_DIR}/${groupRootSubdir}/" )

	_ebsVmsWorkspaceReplaceDynamicRefs( "Group" ${pGroupID} ${groupFullRootDir} groupFullRootDir )
	_ebsVmsWorkspaceSetInternal( FALSE "Group:${pGroupID}" "FullRootDir" ${groupFullRootDir} )

endfunction()

#
function( ebsWorkspacSetProjectPathConfiguration pProjectID outProjectRootSubdir )

    ebsWorkspaceGetProjectGroup( ${pProjectID} projectGroupID )

    ebsVmsWorkspaceQueryEx( "Group:${projectGroupID}" "BaseDir" projectBaseDir isBaseDirSet )
    ebsVmsWorkspaceQueryEx( "Project:${pProjectID}" "RootDir" projectRootDir isProjectRootDirSet )

    if( NOT isProjectRootDirSet )
        ebsVmsWorkspaceQueryEx( "Group:${projectGroupID}" "DefaultRootDir" projectRootDir isProjectRootDirSet )
    endif()

    if( isBaseDirSet )
        set( projectRootSubdir "${projectBaseDir}/${projectRootDir}" )
    else()
        set( projectRootSubdir "${projectRootDir}" )
    endif()

    string( REPLACE "//" "." projectRootSubdir "${projectRootSubdir}" )
    set( projectFullRootDir "${CMAKE_SOURCE_DIR}/${projectRootSubdir}/" )

    _ebsVmsWorkspaceReplaceDynamicRefs( "Project" ${pProjectID} ${projectRootSubdir} projectRootSubdir )
    _ebsVmsWorkspaceSetInternal( FALSE "Project:${pProjectID}" "FullRootDir" ${projectFullRootDir} )

    set( ${outProjectRootSubdir} "${projectRootSubdir}" PARENT_SCOPE )

endfunction()
