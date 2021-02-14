
# Rgistry - automated functionality responsible for storing all variables set within the EBS.
# Used internally by the VMS, can be queried any time to get the list of currently set variables.
# Registry consist of:
# - global list of all EBS variables
# - per-vms-group lists with group-specific vars.

#
function( _ebsRegLogDbgInfo )
	ebsCoreLogDebug( "Registry" "${ARGN}" )
endfunction()
#
function( _ebsRegLogError )
	ebsCoreLogError( "Registry" "${ARGN}" )
endfunction()
#
function( _ebsRegLogDumpInfo )
	ebsCoreLogAlwaysOn( "Registry" "${ARGN}" )
endfunction()


################################################################
################################################################
################################################################

#
function( _ebsRegistryAddEntry pVarCategory pVarSystemName )
    ebsCoreUtilListAppend( _EBS_REGDEF_VARLIST_GLOBAL ${pVarSystemName} XUNIQUE XSORT XCACHE )
    if( pVarCategory )
        ebsCoreUtilListAppend( _EBS_REGDEF_VARLIST_VARCAT_${pVarCategory} ${pVarSystemName} XUNIQUE XSORT XCACHE )
    endif()
endfunction()

#
function( _ebsRegistryRemoveEntry pVarCategory pVarSystemName )
    ebsCoreUtilListRemove( _EBS_REGDEF_VARLIST_GLOBAL ${pVarSystemName} XCACHE )
    if( pVarCategory )
        ebsCoreUtilListRemove( _EBS_REGDEF_VARLIST_VARCAT_${pVarCategory} ${pVarSystemName} XCACHE )
    endif()
endfunction()

#
#
function( ebsRegistryDumpGlobal )
    _ebsRegLogDumpInfo( "[Global registry dump]" )
    if( _EBS_REGDEF_VARLIST_GLOBAL )
        foreach( registeredVariable ${_EBS_REGDEF_VARLIST_GLOBAL} )
            _ebsRegLogDumpInfo( "-- ${registeredVariable} --> ${${registeredVariable}}" )
        endforeach()
    else()
        _ebsRegLogDumpInfo( "-- (empty)" )
    endif()
endfunction()

#
#
function( ebsRegistryDumpVarCategory pVarCategory )
    _ebsRegLogDumpInfo( "[VmsCategory registry dump: ${pVarCategory}]" )
    if( _EBS_REGDEF_VARLIST_VARCAT_${pVarCategory} )
        foreach( registeredVariable ${_EBS_REGDEF_VARLIST_VARCAT_${pVarCategory}} )
            _ebsRegLogDumpInfo( "-- ${registeredVariable} --> ${${registeredVariable}}" )
        endforeach()
    else()
        _ebsRegLogDumpInfo( "-- (empty)" )
    endif()
endfunction()

#
#
function( ebsVmsBaseRegistryResetIndex )
    foreach( vmsCategory ${_EBS_VMSDEF_VARCATEGORYLIST} )
        unset( _EBS_REGDEF_VARLIST_VARCAT_${vmsCategory} CACHE )
    endforeach()
    unset( _EBS_VMS_REG_GLOBAL CACHE )
endfunction()


#
#
function( ebsVmsBaseRegistryResetSystem )
    foreach( vmsCategory ${_EBS_VMSDEF_VARCATEGORYLIST} )
        foreach( registeredVariableName ${_EBS_REGDEF_VARLIST_VARCAT_${vmsCategory}} )
            unset( ${registeredVariableName} CACHE )
        endforeach()
        unset( _EBS_REGDEF_VARLIST_VARCAT_${vmsCategory} CACHE )
    endforeach()
    foreach( globalVariable ${_EBS_REGDEF_VARLIST_GLOBAL} )
        unset( ${globalVariable} CACHE )
    endforeach()
    unset( _EBS_REGDEF_VARLIST_GLOBAL CACHE )
endfunction()
