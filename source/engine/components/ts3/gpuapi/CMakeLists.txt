
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
        "state/gpuStateObject.h"
        "state/gpuStateObject.cpp"
        "state/graphicsPipelineStateCommon.h"
        "state/graphicsPipelineStateDescriptors.h"
        "state/graphicsPipelineStateDescriptors.cpp
        "state/inputAssemblerCommon.h"
        "state/inputAssemblerVertexFormat.h"
        "state/inputAssemblerVertexFormat.cpp"
        "state/inputAssemblerVertexStream.h"
        "state/inputAssemblerVertexStream.cpp"
        "state/graphicsPipelineDescriptor.h"
        "state/graphicsPipelineDescriptor.cpp"
        "state/pipelineDescriptorCache.h"
        "state/pipelineDescriptorCache.cpp"
        "state/pipelineStateController.h"
        "state/pipelineStateController.cpp"

        #"state/pipelineStateDesc.h"
        #"state/pipelineStateDesc.cpp"
        "state/pipelineStateObject.h"
        "state/pipelineStateObject.cpp"
        "state/separablePipelineState.h"
        "state/separablePipelineState.cpp"
        "state/renderPassCommon.h"
        "state/renderTargetCommon.h"
        "state/renderTargetCommon.cpp"
        "state/renderTargetStateObject.h"
        "state/renderTargetStateObject.cpp"
        "state/graphicsShaderBinding.h"
        "state/graphicsShaderBinding.cpp"
        "state/shaderInputSignature.h"
        "state/shaderInputSignature.cpp"
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
        )
