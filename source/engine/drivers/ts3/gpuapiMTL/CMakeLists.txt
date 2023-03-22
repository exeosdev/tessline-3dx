
setInternal( TS3_DRIVER_GPUAPI_MTLCOMMON_SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_prerequisites.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_prerequisites.mm"

        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_apiTranslationLayer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_apiTranslationLayer.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_commandList.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_commandList.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_commandSystem.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_commandSystem.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_gpuDevice.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_gpuDevice.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_gpuDriver.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_gpuDriver.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_presentationLayer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/MTL_presentationLayer.mm"

        "${CMAKE_CURRENT_SOURCE_DIR}/memory/MTL_memoryCommon.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/memory/MTL_memoryCommon.mm"

        "${CMAKE_CURRENT_SOURCE_DIR}/resources/MTL_gpuBuffer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/MTL_gpuBuffer.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/MTL_sampler.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/MTL_sampler.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/MTL_shader.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/MTL_shader.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/MTL_texture.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/MTL_texture.mm"

        "${CMAKE_CURRENT_SOURCE_DIR}/state/MTL_pipelineImmutableStateFactory.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/state/MTL_pipelineImmutableStateFactory.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/state/MTL_pipelineStateController.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/state/MTL_pipelineStateController.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/state/MTL_pipelineStateObject.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/state/MTL_pipelineStateObject.mm"
        )
