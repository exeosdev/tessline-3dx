
set( TS3_DRIVER_GPUAPI_DX12_SRC_FILES
        ${TS3_DRIVER_GPUAPI_DX_SRC_FILES}

        "prerequisites/DX12_coreDefs.h"

        "DX12_prerequisites.h"
        "DX12_prerequisites.cpp"
		
        "DX12_coreAPIProxy.h"
        "DX12_coreAPIProxy.cpp"
        "DX12_gpuCmdContext.h"
        "DX12_gpuCmdContext.cpp"
        "DX12_gpuCmdManager.h"
        "DX12_gpuCmdManager.cpp"
        "DX12_gpuDevice.h"
        "DX12_gpuDevice.cpp"
        "DX12_gpuDriver.h"
        "DX12_gpuDriver.cpp"
        "DX12_gpuDriverAPI.h"
        "DX12_gpuDriverAPI.cpp"
        "DX12_presentationLayer.h"
        "DX12_presentationLayer.cpp"

        )


if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_MSE} )

    if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
        add_library( ts3drv-gpuapi-dx12 STATIC ${TS3_DRIVER_GPUAPI_DX12_SRC_FILES} )
        target_compile_definitions( ts3drv-gpuapi-dx12 PRIVATE
                "TS3_BUILD_STATIC=1")
    else()
        add_library( ts3drv-gpuapi-dx12 SHARED ${TS3_DRIVER_GPUAPI_DX12_SRC_FILES} )
    endif()

    set_target_properties( ts3drv-gpuapi-dx12 PROPERTIES
            OUTPUT_NAME "ts3dx-driver-gpuapi-dx12" )

    target_compile_definitions( ts3drv-gpuapi-dx12 PRIVATE
            "TS3_DRIVER_GPUAPI_DX_BUILD=1"
            "TS3_DRIVER_GPUAPI_DX12_BUILD=1")

    target_include_directories( ts3drv-gpuapi-dx12 PUBLIC
            ${TS3_DRIVERS_INCLUDE_DIR_INTERNAL} )

    target_link_libraries( ts3drv-gpuapi-dx12 PUBLIC
            ts3-gpuapi
            D3D12.lib )

endif()
