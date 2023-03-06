
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
        "gpuDriver.h"
        "gpuDriver.cpp"
        "gpuDriverAPI.h"
        "gpuDriverPlugin.h"
        "gpuUtils.h"
        "gpuUtils.cpp"
        "gpuCapabilities.h"
        "presentationLayer.h"
        "presentationLayer.cpp"
        # "systemDisplayManager.h"
        # "systemDisplayManager.cpp"

        "memory/commonGPUMemoryDefs.h"
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
        "resources/renderBuffer.h"
        "resources/renderBuffer.cpp"
        "resources/renderTargetTexture.h"
        "resources/renderTargetTexture.cpp"
        "resources/sampler.h"
        "resources/sampler.cpp"
        "resources/samplerCommon.h"
        "resources/samplerCommon.cpp"
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

        "state/descriptors/iaVertexStreamDescriptors.h"
        "state/descriptors/iaVertexStreamDescriptors.cpp"
        "state/descriptors/iaVertexStreamDescriptorSet.h"
        "state/descriptors/iaVertexStreamDescriptorSet.cpp"
        "state/descriptors/renderPassAttachmentDescriptors.h"
        "state/descriptors/renderPassAttachmentDescriptors.cpp"
        "state/descriptors/renderPassAttachmentDescriptorSet.h"
        "state/descriptors/renderPassAttachmentDescriptorSet.cpp"

        "state/immutable/commonGraphicsConfigImmutableStates.h"
        "state/immutable/commonGraphicsConfigImmutableStates.cpp"
        "state/immutable/graphicsShaderLinkageImmutableState.h"
        "state/immutable/graphicsShaderLinkageImmutableState.cpp"
        "state/immutable/inputAssemblerDynamicStates.h"
        "state/immutable/inputAssemblerDynamicStates.cpp"
        "state/immutable/inputAssemblerImmutableStates.h"
        "state/immutable/inputAssemblerImmutableStates.cpp"
        "state/immutable/renderTargetDynamicStates.h"
        "state/immutable/renderTargetDynamicStates.cpp"
        "state/immutable/renderTargetImmutableStates.h"
        "state/immutable/renderTargetImmutableStates.cpp"
        "state/immutable/separablePipelineImmutableState.h"
        "state/immutable/separablePipelineImmutableState.cpp"

        "state/commonGPUStateDefs.h"
        "state/commonGPUStateDefs.cpp"
        "state/commonGraphicsConfig.h"
        "state/commonGraphicsConfig.cpp"
        "state/graphicsPipelineImmutableState.h"
        "state/graphicsPipelineImmutableState.cpp"
        "state/inputAssemblerCommon.h"
        "state/inputAssemblerCommon.cpp"
        "state/graphicsPipelineStateController.h"
        "state/graphicsPipelineStateController.cpp"
        "state/pipelineImmutableStateCache.h"
        "state/pipelineImmutableStateCache.cpp"

        "state/pipelineStateObject.h"
        "state/pipelineStateObject.cpp"
        "state/separablePipelineState.h"
        "state/separablePipelineState.cpp"
        "state/renderPassCommon.h"
        "state/renderTargetCommon.h"
        "state/renderTargetCommon.cpp"
        "state/renderTargetStateObject.h"
        "state/renderTargetStateObject.cpp"
        "state/shaderInputSignature.h"
        "state/shaderInputSignature.cpp"
        "state/graphicsShaderState.h"
        "state/graphicsShaderState.cpp"
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
