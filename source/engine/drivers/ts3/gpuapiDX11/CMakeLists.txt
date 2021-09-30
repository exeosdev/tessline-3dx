
set( TS3_DRIVER_GPUAPI_DX11_SRC_FILES
        ${TS3_DRIVER_GPUAPI_DX_SRC_FILES}

        "DX11_prerequisites.h"
        "DX11_prerequisites.cpp"
		
        "DX11_commandList.h"
        "DX11_commandList.cpp"
        "DX11_commandSystem.h"
        "DX11_commandSystem.cpp"
        "DX11_coreAPIProxy.h"
        "DX11_coreAPIProxy.cpp"
        "DX11_gpuDevice.h"
        "DX11_gpuDevice.cpp"
        "DX11_gpuDriver.h"
        "DX11_gpuDriver.cpp"
        "DX11_gpuDriverAPI.h"
        "DX11_gpuDriverAPI.cpp"
        "DX11_presentationLayer.h"
        "DX11_presentationLayer.cpp"


        "resources/DX11_gpuBuffer.h"
        "resources/DX11_gpuBuffer.cpp"
        "resources/DX11_renderBuffer.h"
        "resources/DX11_renderBuffer.cpp"
        "resources/DX11_sampler.h"
        "resources/DX11_sampler.cpp"
        "resources/DX11_shader.h"
        "resources/DX11_shader.cpp"
        "resources/DX11_texture.h"
        "resources/DX11_texture.cpp"

        "state/DX11_pipelineStateController.h"
        "state/DX11_pipelineStateController.cpp"
        "state/DX11_pipelineStateObject.h"
        "state/DX11_pipelineStateObject.cpp"
        "state/DX11_pipelineStateDesc.h"
        "state/DX11_pipelineStateDesc.cpp"
        "state/DX11_renderTargetStateObject.h"
        "state/DX11_renderTargetStateObject.cpp"
        "state/DX11_vertexStreamStateObject.h"
        "state/DX11_vertexStreamStateObject.cpp"

        )


if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_MSE} )

    if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
        add_library( ts3drv-gpuapi-dx11 STATIC ${TS3_DRIVER_GPUAPI_DX11_SRC_FILES} )
        target_compile_definitions( ts3drv-gpuapi-dx11 PRIVATE
                "TS3_BUILD_STATIC=1")
    else()
        add_library( ts3drv-gpuapi-dx11 SHARED ${TS3_DRIVER_GPUAPI_DX11_SRC_FILES} )
    endif()

    set_target_properties( ts3drv-gpuapi-dx11 PROPERTIES
            OUTPUT_NAME "ts3dx-driver-gpuapi-dx11" )

    target_compile_definitions( ts3drv-gpuapi-dx11 PRIVATE
            "TS3_DRIVER_GPUAPI_DX_BUILD=1"
            "TS3_DRIVER_GPUAPI_DX11_BUILD=1")

    target_include_directories( ts3drv-gpuapi-dx11 PUBLIC
            ${TS3_DRIVERS_INCLUDE_DIR_INTERNAL} )

    target_link_libraries( ts3drv-gpuapi-dx11 PUBLIC
            ts3-gpuapi
            D3D11.lib
            D3DCompiler.lib )

endif()
