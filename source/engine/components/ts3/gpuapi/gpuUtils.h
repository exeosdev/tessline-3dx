
#pragma once

#ifndef __TS3_GPUAPI_GPU_UTILS_H__
#define __TS3_GPUAPI_GPU_UTILS_H__

#include "resources/shaderCommon.h"

namespace ts3::GpuAPI
{

    namespace utils
    {

        TS3_GPUAPI_API ShaderHandle createShaderFromSource( GPUDevice & pGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength );

        inline ShaderHandle createShaderFromSource( GPUDevice & pGPUDevice, EShaderType pShaderType, const std::string & pSource )
        {
            return createShaderFromSource( pGPUDevice, pShaderType, pSource.c_str(), pSource.length() );
        }

        TS3_GPUAPI_API ShaderHandle createShaderFromFile( GPUDevice & pGPUDevice, EShaderType pShaderType, const char * pFilename );

        inline ShaderHandle createShaderFromFile( GPUDevice & pGPUDevice, EShaderType pShaderType, const std::string & pFilename )
        {
            return createShaderFromFile( pGPUDevice, pShaderType, pFilename.c_str() );
        }

    }

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_GPU_UTILS_H__
