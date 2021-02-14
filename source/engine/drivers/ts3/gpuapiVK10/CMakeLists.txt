
set( TS3_DRIVER_GPUAPI_VK10_SRC_FILES
        ${TS3_DRIVER_GPUAPI_VKCOMMON_SRC_FILES}

        "VK10_prerequisites.h"
        "VK10_prerequisites.cpp"

        "core/VK10_gpuDriverAPI.h"
        "core/VK10_gpuDriverAPI.cpp"
        )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3drv-gpuapi-vk10 STATIC ${TS3_DRIVER_GPUAPI_VK10_SRC_FILES} )
    target_compile_definitions( ts3drv-gpuapi-vk10 PRIVATE
            "TS3_BUILD_STATIC=1")
else()
    add_library( ts3drv-gpuapi-vk10 SHARED ${TS3_DRIVER_GPUAPI_VK10_SRC_FILES} )
endif()

set_target_properties( ts3drv-gpuapi-vk10 PROPERTIES
        OUTPUT_NAME "ts3dx-driver-gpuapi-vk10" )

target_compile_definitions( ts3drv-gpuapi-vk10 PRIVATE
        "TS3_DRIVER_GPUAPI_VKCOMMON_BUILD=1"
        "TS3_DRIVER_GPUAPI_VK10_BUILD=1")

target_include_directories( ts3drv-gpuapi-vk10 PUBLIC
        ${TS3_DRIVERS_INCLUDE_DIR_INTERNAL} )

target_link_libraries( ts3drv-gpuapi-vk10 PUBLIC
        ts3-gpuapi )
