
set( TS3_GPUAPI_SRC_FILES
        "prerequisites/commonTypes.h"
        "prerequisites/coreInterfaceDefs.h"
        "prerequisites/coreInterfaceDefs.cpp"
        "prerequisites/gpuBaseObject.h"
        "prerequisites/gpuBaseObject.cpp"
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

        "pdesc/pipelineDescriptorsCommon.h"

        "resources/commonGPUResourceDefs.h"
        "resources/gpuBuffer.h"
        "resources/gpuBuffer.cpp"
        "resources/gpuBufferCommon.h"
        "resources/gpuResource.h"
        "resources/gpuResource.cpp"
        "resources/renderBuffer.h"
        "resources/renderBuffer.cpp"
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

        "state/commonGPUStateDefs.h"
        "state/commonGPUStateDefs.cpp"
        "state/gpuStateObject.h"
        "state/gpuStateObject.cpp"
        "state/graphicsPipelineConfigDefs.h"
        "state/graphicsPipelineConfigDefs.cpp"
        "state/graphicsPipelineState.h"
        "state/graphicsPipelineState.cpp"
        "state/inputAssemblerCommon.h"
        "state/inputAssemblerCommon.cpp"
        "state/inputAssemblerVertexFormat.h"
        "state/inputAssemblerVertexFormat.cpp"
        "state/inputAssemblerVertexStream.h"
        "state/inputAssemblerVertexStream.cpp"
        "state/pipelineDescriptorCache.h"
        "state/pipelineDescriptorCache.cpp"
        "state/graphicsPipelineStateController.h"
        "state/graphicsPipelineStateController.cpp"

        #"state/pipelineStateDesc.h"
        #"state/pipelineStateDesc.cpp"
        "state/pipelineStateObject.h"
        "state/pipelineStateObject.cpp"
        "state/separableGraphicsShaderLinkage.h"
        "state/separableGraphicsShaderLinkage.cpp"
        "state/separablePipelineState.h"
        "state/separablePipelineState.cpp"
        "state/renderPassCommon.h"
        "state/renderTargetCommon.h"
        "state/renderTargetCommon.cpp"
        "state/renderTargetStateObject.h"
        "state/renderTargetStateObject.cpp"
        "state/graphicsShaderLinkage.h"
        "state/graphicsShaderLinkage.cpp"
        "state/shaderInputSignature.h"
        "state/shaderInputSignature.cpp"
        )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3-GpuAPI STATIC ${TS3_GPUAPI_SRC_FILES} )
    target_compile_definitions( ts3-GpuAPI PRIVATE
            "TS3_BUILD_STATIC=1" )
else()
    add_library( ts3-GpuAPI SHARED ${TS3_GPUAPI_SRC_FILES} )
endif()

set_target_properties( ts3-GpuAPI PROPERTIES
        OUTPUT_NAME "ts3-GpuAPI"
        )

target_compile_definitions( ts3-GpuAPI PRIVATE
        "TS3_GPUAPI_BUILD=1"
        )

target_include_directories( ts3-GpuAPI PUBLIC
        "${TS3_INCLUDE_DIR}"
        )

target_link_libraries( ts3-GpuAPI PUBLIC
        ts3-core
        ts3-math
        )
