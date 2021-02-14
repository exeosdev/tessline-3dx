
set( TS3_DRIVER_GPUAPI_GLES3_SRC_FILES
        ${TS3_DRIVER_GPUAPI_GLCOMMON_SRC_FILES}

        "GLES3_prerequisites.h"
        "GLES3_gpuCmdCore.h"
        "GLES3_gpuDevice.h"
        "GLES3_gpuDevice.cpp"
        "GLES3_gpuDriver.h"
        "GLES3_gpuDriver.cpp"
)

if( "${TS3_VAR_ENGINE_BUILD_MODE_DRIVERS}" STREQUAL "STATIC" )
    add_library( ts3_dgxgl4 STATIC ${TS3_DRIVER_GPUAPI_GLES3_SRC_FILES} )
    target_compile_definitions( ts3_dgxgl4 PRIVATE
        "TS3_BUILD_STATIC=1")
else()
    add_library( ts3_dgxgl4 SHARED ${TS3_DRIVER_GPUAPI_GLES3_SRC_FILES} )
endif()

set_target_properties( ts3_dgxgl4 PROPERTIES
    OUTPUT_NAME "ts3dx-driver-gpuapi-gl4" )

target_compile_definitions( ts3_dgxgl4 PRIVATE
        "TS3_DRIVER_GPUAPI_GL_BUILD=1"
        "TS3_DRIVER_GPUAPI_GLES3_BUILD=1"
        "GL_PLATFORM_TYPE_ES=1" )

target_include_directories( ts3_dgxgl4 PUBLIC
    ${TS3_DRIVERS_INCLUDE_DIR_INTERNAL} )

target_link_libraries( ts3_dgxgl4 PUBLIC
    ts3_ecgraphics )
