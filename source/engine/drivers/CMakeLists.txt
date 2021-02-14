
set( TS3_DRIVERS_INCLUDE_DIR
        "${TS3_INCLUDE_DIR}"
        "${TS3_BASE_DIR}/source/engine/components" )
		
set( TS3_DRIVERS_INCLUDE_DIR_INTERNAL
        ${TS3_DRIVERS_INCLUDE_DIR}
        "${TS3_BASE_DIR}/source/engine/drivers" )

add_subdirectory( "ts3/gpuapiGL" )
add_subdirectory( "ts3/gpuapiGL4" )
#add_subdirectory( "ts3/gpuapiGLES3" )
add_subdirectory( "ts3/gpuapiDX" )
add_subdirectory( "ts3/gpuapiDX11" )
#add_subdirectory( "ts3/gpuapiDX12" )
#add_subdirectory( "ts3/gpuapiVK" )
#add_subdirectory( "ts3/gpuapiVK10" )
