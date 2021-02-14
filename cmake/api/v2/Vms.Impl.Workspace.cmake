

#
function( _ebsWorkspaceCheckFilterMatch outMatchResult )
    # This will parse filter string (e.g. "Compiler=GCC System=iOS;macOS") to list of values ("Compiler:GCC;System:iOS;System:macOS")
    ebsCoreFilterParseVarargInput( filterStringList "${ARGN}" )
    # Check if specified filters match the current platform setup.
    ebsCoreFilterCheckPlatformMatch( "${filterStringList}" matchResult )
    # Set the result of matching.
    set( ${outMatchResult} ${matchResult} PARENT_SCOPE )
endfunction()

#
function( _ebsWorkspaceParseItemRefName pItemRefName outItemType outWorkspaceItemID )
    if( ${pItemRefName} MATCHES "Config|Global" )
        set( ${outItemType} ${pItemRefName} PARENT_SCOPE )
        return()
    endif()

    set( possibleValuesForItemName
        "Config"
        "Global"
        "$GroupName"
        "$GroupName/$ProjectName" )

    # Handle '/', which may be used as a "scope" separator for the project.
    ebsCoreUtilStringSplit( ${pItemRefName} "." substrList )
    list( LENGTH substrList listLength )

    # Single identifier means group (Config|Global already checked).
    if( ${listLength} EQUAL 1 )
        set( ${outItemType} "Group" PARENT_SCOPE )
        set( ${outWorkspaceItemID} "GID:${pItemRefName}" PARENT_SCOPE )
        return()
    endif()

    # Two identifiers separated with '/' is a project ID.
    if( ${listLength} EQUAL 2 )
        list( GET substrList 0 groupName )
        list( GET substrList 1 projectName )
        set( ${outItemType} "Project" PARENT_SCOPE )
        set( ${outWorkspaceItemID} "PID:${groupName}:${projectName}" PARENT_SCOPE )
        return()
    endif()

    # This is either incorrect format or to many separators.
    ebsVmsLogError( "Invalid workspace item name: ${pItemRefName}. Possible values: ${possibleValuesForItemName}." )
endfunction()

function( _ebsVmsWorkspaceSet pInternalMode pItemRefName pPropertyName pValue )
    _ebsWorkspaceCheckFilterMatch( filterMatch "${ARGN}" )
    if( filterMatch )
        _ebsWorkspaceParseItemRefName( ${pItemRefName} itemType itemID )
        message( "itemType: ${itemType} // itemID: ${itemID}" )
        #if( pCheckRegStatus )
        #    _ebsVmsWorkspaceCheckRegStatus( ${itemType} "${itemID}" isItemRegistered )
        #endif()
        #if( NOT pCheckRegStatus OR isItemRegistered )
            # _ebsVmsWorkspaceGetPropertyVarRefName( ${itemType} ${pPropertyName} varRefName )
            # _ebsVmsSetImpl( "${pInternalMode}" "Workspace.${varRefName}" "${vmsItemID}" "${pValue}" )
        #endif()
    endif()
endfunction()


function( _ebsVmsWorkspaceQueryParentGroup pItemRefName outGroupID )
    string( FIND "${pItemRefName}" "." separatorPos REVERSE )
    if( ${separatorPos} EQUAL -1 )
        set( ${outGroupID} "" PARENT_SCOPE )
    else()
        string( SUBSTRING "${pItemRefName}" 0 ${separatorPos} groupName )
        set( ${outGroupID} "${groupName}" PARENT_SCOPE )
    endif()
endfunction()


function( _ebsVmsWorkspaceFetchItemID pRefID )
    _ebsVmsCmdGet( "Workspace.CoreItemList.UIDMap" IGNORE itemUIDMap )
    foreach( uidMapEntry ${itemUIDMap} )
        ebsCoreUtilStringSplit( "${uidMapEntry}" ":"  )
    endforeach()
endfunction()


# By default global group UID is: GID:${pOptGroupUID}
function( _ebsVmsWorkspaceRegisterItemBase pItemID pUserRefID outParentGroup )
    _ebsVmsWorkspaceQueryParentGroup( ${pItemID} parentGroup )
    if( parentGroup )
        _ebsVmsWorkspaceRegisterGroupEx( "${parentGroup}" IGNORE )
        _ebsVmsCmdSetInternal( "Workspace.CoreItemProp.ParentGroupID" ${pItemID} "${parentGroup}" )
    endif()
    set( ${outParentGroup} ${parentGroup} PARENT_SCOPE )
    if( pUserRefID )
        _ebsVmsCmdAppendInternal( "Workspace.CoreItemList.UIDMap" IGNORE "${pUserRefID}:${pItemID}" )
    endif()
endfunction()


# By default global group UID is: GID:${pOptGroupUID}
function( _ebsVmsWorkspaceRegisterGroupEx pGroupID pUserRefID )
    _ebsVmsWorkspaceRegisterItemBase( ${pGroupID} ${pUserRefID} parentGroup )
    _ebsVmsCmdAppendInternal( "Workspace.CoreItemProp.SubGroupList" "${parentGroup}" "${pGroupID}" )
endfunction()

macro( _ebsVmsWorkspaceRegisterGroup pGroupID )
    _ebsVmsWorkspaceRegisterGroupEx( ${pGroupID} IGNORE )
endmacro()

# By default global project PID is: GID:${pOptGroupUID}
function( _ebsVmsWorkspaceRegisterProjectEx pProjectID pUserRefID )
    _ebsVmsWorkspaceRegisterItemBase( ${pProjectID} ${pUserRefID} parentGroup )
    _ebsVmsCmdAppendInternal( "Workspace.CoreItemProp.SubProjectList" "${parentGroup}" "${pProjectID}" )
    if( pGlobalUID )
    endif()
endfunction()

macro( _ebsVmsWorkspaceRegisterProject pProjectID )
    _ebsVmsWorkspaceRegisterProjectEx( ${pProjectID} IGNORE )
endmacro()
