
set( TS3_DRIVER_GPUAPI_MTL1_SRC_FILES
        ${TS3_DRIVER_GPUAPI_MTLCOMMON_SRC_FILES}

        "MTL1_prerequisites.h"
        "MTL1_gpuDevice.h"
        "MTL1_gpuDevice.mm"
        "MTL1_gpuDriver.h"
        "MTL1_gpuDriver.mm"
        "MTL1_gpuDriverAPI.h"
        "MTL1_gpuDriverAPI.mm"
        )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3drv-gpuapi-mtl1 STATIC ${TS3_DRIVER_GPUAPI_MTL1_SRC_FILES} )
    target_compile_definitions( ts3drv-gpuapi-mtl1 PRIVATE
            "TS3_BUILD_STATIC=1")
else()
    add_library( ts3drv-gpuapi-mtl1 SHARED ${TS3_DRIVER_GPUAPI_MTL1_SRC_FILES} )
endif()

set_target_properties( ts3drv-gpuapi-mtl1 PROPERTIES
        OUTPUT_NAME "ts3dx-driver-gx-mtl1" )

target_compile_definitions( ts3drv-gpuapi-mtl1 PRIVATE
        "TS3_DRIVER_GPUAPI_MTL_BUILD=1"
        "TS3_DRIVER_GPUAPI_MTL1_BUILD=1" )

target_include_directories( ts3drv-gpuapi-mtl1 PUBLIC
        ${TS3_DRIVERS_INCLUDE_DIR_INTERNAL} )

target_link_libraries( ts3drv-gpuapi-mtl1 PUBLIC
        ts3-gpuapi
        ts3-system )
