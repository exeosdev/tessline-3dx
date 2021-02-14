
#pragma once

#ifndef __TS3_GPUAPI_GPU_UTILS_H__
#define __TS3_GPUAPI_GPU_UTILS_H__

#include "resources/shaderCommon.h"

namespace ts3::gpuapi::Utils
{

	TS3_GPUAPI_API ShaderHandle createShaderFromFile( GPUDevice & pGPUDevice, EShaderType pShaderType, const char * pFilename );

	inline ShaderHandle createShaderFromFile( GPUDevice & pGPUDevice, EShaderType pShaderType, const std::string & pFilename )
	{
		return createShaderFromFile( pGPUDevice, pShaderType, pFilename.c_str() );
	}

}

#endif // __TS3_GPUAPI_GPU_UTILS_H__
