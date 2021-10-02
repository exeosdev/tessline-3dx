
set( TS3_DRIVER_GPUAPI_GL4_SRC_FILES
        ${TS3_DRIVER_GPUAPI_GLCOMMON_SRC_FILES}

        "GL4_prerequisites.h"
        "GL4_gpuCmdCore.h"
        "GL4_gpuDevice.h"
        "GL4_gpuDevice.cpp"
        "GL4_gpuDriver.h"
        "GL4_gpuDriver.cpp"
        "GL4_gpuDriverAPI.h"
        "GL4_gpuDriverAPI.cpp"
        )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
        add_library( ts3drv-gpuapi-gl4 STATIC ${TS3_DRIVER_GPUAPI_GL4_SRC_FILES} )
        target_compile_definitions( ts3drv-gpuapi-gl4 PRIVATE
            "TS3_BUILD_STATIC=1")
else()
        add_library( ts3drv-gpuapi-gl4 SHARED ${TS3_DRIVER_GPUAPI_GL4_SRC_FILES} )
endif()

set_target_properties( ts3drv-gpuapi-gl4 PROPERTIES
        OUTPUT_NAME "ts3dx-driver-gx-gl4" )

target_compile_definitions( ts3drv-gpuapi-gl4 PRIVATE
        #"GLEW_STATIC=1"
        "TS3_DRIVER_GPUAPI_GL_BUILD=1"
        "TS3_DRIVER_GPUAPI_GL4_BUILD=1"
        "GL_PLATFORM_TYPE_CORE=1" )

target_include_directories( ts3drv-gpuapi-gl4 PUBLIC
        ${TS3_DRIVERS_INCLUDE_DIR_INTERNAL} )

target_link_libraries( ts3drv-gpuapi-gl4 PUBLIC
        ts3-gpuapi
        ts3-system )
