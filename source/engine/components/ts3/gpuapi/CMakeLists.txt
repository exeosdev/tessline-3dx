
set( TS3_GPUAPI_SRC_FILES
        "prerequisites/commonTypes.h"
        "prerequisites/coreInterfaceDefs.h"
        "prerequisites/coreInterfaceDefs.cpp"
        "prerequisites/gpuDataFormats.h"
        "prerequisites/gpuDataFormats.cpp"

        "prerequisites.h"
        "prerequisites.cpp"

        "commonCommandDefs.h"
        "commandContext.h"
        "commandContext.cpp"
        "commandList.h"
        "commandList.cpp"
        "commandSystem.h"
        "commandSystem.cpp"
        "displayCommon.h"
        "displayManager.h"
        "displayManager.cpp"
        "gpuDevice.h"
        "gpuDevice.cpp"
        "gpuDeviceNull.h"
        "gpuDriver.h"
        "gpuDriver.cpp"
        "gpuDriverAPI.h"
        "gpuDriverNull.h"
        "gpuDriverPlugin.h"
        "gpuUtils.h"
        "gpuUtils.cpp"
        "gpuCapabilities.h"
        "presentationLayer.h"
        "presentationLayer.cpp"
        # "systemDisplayManager.h"
        # "systemDisplayManager.cpp"

        "memory/commonGPUMemoryDefs.h"
        "memory/commonGPUMemoryDefs.cpp"
        "memory/gpuMemoryAllocator.h"
        "memory/gpuMemoryAllocator.cpp"
        "memory/gpuMemoryHeap.h"
        "memory/gpuMemoryHeap.cpp"
        "memory/gpuMemoryPool.h"
        "memory/gpuMemoryPool.cpp"
        "memory/gpuMemoryRef.h"
        "memory/gpuMemoryRef.cpp"

        "resources/commonGPUResourceDefs.h"
        "resources/gpuBuffer.h"
        "resources/gpuBuffer.cpp"
        "resources/gpuBufferReference.h"
        "resources/gpuBufferReference.cpp"
        "resources/gpuBufferCommon.h"
        "resources/gpuBufferCommon.cpp"
        "resources/gpuResource.h"
        "resources/gpuResource.cpp"
        "resources/renderTargetTexture.h"
        "resources/renderTargetTexture.cpp"
        "resources/shader.h"
        "resources/shader.cpp"
        "resources/shaderCommon.h"
        "resources/texture.h"
        "resources/texture.cpp"
        "resources/textureCommon.h"
        "resources/textureCommon.cpp"
        "resources/textureDimensions.h"
        "resources/textureDimensions.cpp"
        "resources/textureReference.h"
        "resources/textureReference.cpp"

        "state/commonGPUStateDefs.h"
        "state/commonGPUStateDefs.cpp"
        "state/commonGraphicsConfig.h"
        "state/commonGraphicsConfig.cpp"
        "state/commonGraphicsConfigImmutableStates.h"
        "state/commonGraphicsConfigImmutableStates.cpp"

        "state/graphicsPipelineImmutableState.h"
        "state/graphicsPipelineImmutableState.cpp"
        "state/graphicsShaderLinkageImmutableState.h"
        "state/graphicsShaderLinkageImmutableState.cpp"

        "state/inputAssemblerCommon.h"
        "state/inputAssemblerCommon.cpp"
        "state/inputAssemblerDynamicStates.h"
        "state/inputAssemblerDynamicStates.cpp"
        "state/inputAssemblerImmutableStates.h"
        "state/inputAssemblerImmutableStates.cpp"

        "state/graphicsPipelineStateController.h"
        "state/graphicsPipelineStateController.cpp"
        "state/graphicsShaderState.h"
        "state/graphicsShaderState.cpp"
        "state/pipelineImmutableStateCache.h"
        "state/pipelineImmutableStateCache.cpp"
        "state/pipelineStateObject.h"
        "state/pipelineStateObject.cpp"
        "state/separablePipelineState.h"
        "state/separablePipelineState.cpp"
        "state/shaderInputSignature.h"
        "state/shaderInputSignature.cpp"

        "state/renderPassCommon.h"
        "state/renderPassCommon.cpp"
        "state/renderTargetCommon.h"
        "state/renderTargetCommon.cpp"
        "state/renderTargetDynamicStates.h"
        "state/renderTargetDynamicStates.cpp"
        "state/renderTargetImmutableStates.h"
        "state/renderTargetImmutableStates.cpp"

        "state/sampler.h"
        "state/sampler.cpp"
        "state/samplerCommon.h"
        "state/samplerCommon.cpp"
        "resources/shaderInputTexture.cpp"
        "resources/shaderInputTexture.h"
        )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3-gpuapi STATIC ${TS3_GPUAPI_SRC_FILES} )
    target_compile_definitions( ts3-gpuapi PRIVATE
            "TS3_BUILD_STATIC=1" )
else()
    add_library( ts3-gpuapi SHARED ${TS3_GPUAPI_SRC_FILES} )
endif()

set_target_properties( ts3-gpuapi PROPERTIES
        OUTPUT_NAME "ts3-gpuapi"
        )

target_compile_definitions( ts3-gpuapi PRIVATE
        "TS3_GPUAPI_BUILD=1"
        )

target_include_directories( ts3-gpuapi PUBLIC
        "${TS3_INCLUDE_DIR}"
        )

target_link_libraries( ts3-gpuapi PUBLIC
        ts3-core
        ts3-math
        ts3-stdext
        )
