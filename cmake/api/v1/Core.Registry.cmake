
# Rgistry - automated functionality responsible for storing all variables set within the EBS.
# Used internally by the VMS, can be queried any time to get the list of currently set variables.
# Registry consist of:
# - global list of all EBS variables
# - per-vms-group lists with group-specific vars.

#
macro( ebsLogRegistryDbginfo )
    ebsLogDbginfo( "REGISTRY" ${ARGN} )
endmacro()
#
macro( ebsLogRegistryError )
    ebsLogError( "REGISTRY" ${ARGN} )
endmacro()
#
macro( ebsLogRegistryDumpInfo )
    ebsLogAlwaysOn( "REGISTRY" ${ARGN} )
endmacro()


################################################################
################################################################
################################################################

#
function( _ebsRegistryAddEntry pVarCategory pVarnameSystem )
    ebsCommonUtilListAppend( _EBS_REGDEF_VARLIST_GLOBAL ${pVarnameSystem} XCACHE XUNIQUE XSORT )
    ebsCommonUtilListAppend( _EBS_REGDEF_VARLIST_VARCAT_${pVarCategory} ${pVarnameSystem} XCACHE XUNIQUE XSORT )
endfunction()

#
function( _ebsRegistryRemoveEntry pVarCategory pVarnameSystem )
    ebsUtilCommonListRemove( _EBS_REGDEF_VARLIST_GLOBAL ${pVarnameSystem} XCACHE XUNIQUE XSORT )
    ebsUtilCommonListRemove( _EBS_REGDEF_VARLIST_VARCAT_${pVarCategory} ${pVarnameSystem} XCACHE XUNIQUE XSORT )
endfunction()

#
#
function( ebsRegistryDumpGlobal )
    ebsLogRegistryDumpInfo( "[Global registry dump]" )
    if( _EBS_REGDEF_VARLIST_GLOBAL )
        foreach( registeredVariable ${_EBS_REGDEF_VARLIST_GLOBAL} )
            ebsLogRegistryDumpInfo( "-- ${registeredVariable} --> ${${registeredVariable}}" )
        endforeach()
    else()
        ebsLogRegistryDumpInfo( "-- (empty)" )
    endif()
endfunction()

#
#
function( ebsVmsBaseRegistryDumpVarCategory pVarCategory )
    ebsLogRegistryDumpInfo( "[VmsCategory registry dump: ${pVarCategory}]" )
    if( _EBS_REGDEF_VARLIST_VARCAT_${pVarCategory} )
        foreach( registeredVariable ${_EBS_REGDEF_VARLIST_VARCAT_${pVarCategory}} )
            ebsLogRegistryDumpInfo( "-- ${registeredVariable} --> ${${registeredVariable}}" )
        endforeach()
    else()
        ebsLogRegistryDumpInfo( "-- (empty)" )
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
