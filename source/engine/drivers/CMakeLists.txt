
set( TS3_DRIVERS_INCLUDE_DIR
        "${TS3_INCLUDE_DIR}"
        "${TS3_BASE_DIR}/source/engine/components" )
		
set( TS3_DRIVERS_INCLUDE_DIR_INTERNAL
        ${TS3_DRIVERS_INCLUDE_DIR}
        "${TS3_BASE_DIR}/source/engine/drivers" )

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Android" )
    set( _EBS_ENGINE_DRIVERS_TARGET_LIST
            "gpuapiGL"
            "gpuapiGLES3"
            )
elseif( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Linux" )
    set( _EBS_ENGINE_DRIVERS_TARGET_LIST
            "gpuapiGL"
            "gpuapiGL4"
            #"gpuapiVK"
            #"gpuapiVK11"
            )
elseif( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "macOS" )
    set( _EBS_ENGINE_DRIVERS_TARGET_LIST
            "gpuapiGL"
            "gpuapiGL4"
            "gpuapiMTL"
            "gpuapiMTL1"
            )
elseif( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "iOS" )
    set( _EBS_ENGINE_DRIVERS_TARGET_LIST
            # "gpuapiMTL"
            # "gpuapiMTL3"
            )
elseif( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "WinDesktop" )
    set( _EBS_ENGINE_DRIVERS_TARGET_LIST
            "gpuapiGL"
            "gpuapiGL4"
            #"gpuapiDX"
            #"gpuapiDX11"
            #"gpuapiDX12"
            )
elseif( "${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_MSE}" )
    set( _EBS_ENGINE_DRIVERS_TARGET_LIST
            "gpuapiDX"
            "gpuapiDX11"
            "gpuapiDX12"
            )
endif()

setInternal( CM_EBS__ENGINE_DRIVERS_TARGET_LIST "${_EBS_ENGINE_DRIVERS_TARGET_LIST}" )

foreach( target ${_EBS_ENGINE_DRIVERS_TARGET_LIST} )
    add_subdirectory( "ts3/${target}" )
    if( TARGET "ts3drv-${target}" )
        set_property( TARGET "ts3drv-${target}" PROPERTY CXX_STANDARD 17 )
        target_compile_features( "ts3drv-${target}" PRIVATE cxx_std_17 )
    endif()
endforeach()
