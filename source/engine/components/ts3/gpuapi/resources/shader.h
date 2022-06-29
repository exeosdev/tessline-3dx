
#pragma once

#ifndef __TS3_GPUAPI_SHADER_H__
#define __TS3_GPUAPI_SHADER_H__

#include "shaderCommon.h"

namespace ts3::gpuapi
{

	class TS3_GPUAPI_CLASS Shader : public GPUBaseObject
	{
	public:
		EShaderType const mShaderType;
		ShaderBinary const mShaderBinary;

		Shader( GPUDevice & pGPUDevice, EShaderType pShaderType );
		Shader( GPUDevice & pGPUDevice, EShaderType pShaderType, ShaderBinary pShaderBinary );

		virtual ~Shader();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SHADER_H__
