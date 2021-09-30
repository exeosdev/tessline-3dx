
set( TS3_DRIVERS_INCLUDE_DIR
        "${TS3_INCLUDE_DIR}"
        "${TS3_BASE_DIR}/source/engine/components" )
		
set( TS3_DRIVERS_INCLUDE_DIR_INTERNAL
        ${TS3_DRIVERS_INCLUDE_DIR}
        "${TS3_BASE_DIR}/source/engine/drivers" )

set( _EBS_ENGINE_DRIVERS_TARGET_LIST
        "gpuapiGL"
        "gpuapiGL4"
        "gpuapiGLES3"
        # "gpuapiDX"
        # "gpuapiDX11"
        # "gpuapiDX12"
        # "gpuapiVK"
        # "gpuapiVK11"
        )

foreach( target ${_EBS_ENGINE_DRIVERS_TARGET_LIST} )
    add_subdirectory( "ts3/${target}" )
    if( TARGET "ts3drv-${target}" )
        set_property( TARGET "ts3drv-${target}" PROPERTY CXX_STANDARD 17 )
        target_compile_features( "ts3drv-${target}" PRIVATE cxx_std_17 )
    endif()
endforeach()
