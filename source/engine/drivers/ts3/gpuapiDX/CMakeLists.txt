
setInternal( TS3_DRIVER_GPUAPI_DX_SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/prerequisites/DX_commonShaderDefs.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/prerequisites/DX_errorUtils.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/prerequisites/DX_errorUtils.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/DX_prerequisites.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_prerequisites.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_apiTranslationLayer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_apiTranslationLayer.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_gpuDevice.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_gpuDevice.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_gpuDriver.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_gpuDriver.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_presentationLayer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_presentationLayer.cpp"
)
