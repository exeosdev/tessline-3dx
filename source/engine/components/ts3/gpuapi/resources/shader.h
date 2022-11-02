
#pragma once

#ifndef __TS3_GPUAPI_SHADER_H__
#define __TS3_GPUAPI_SHADER_H__

#include "commonGPUResourceDefs.h"
#include "shaderCommon.h"

namespace ts3::GpuAPI
{

	class TS3_GPUAPI_CLASS Shader : public GPUBaseObject
	{
	public:
		EShaderType const mShaderType;

		resource_id_t const mID;

		std::unique_ptr<ShaderBinary> const mShaderBinary;

	public:
		Shader(
			GPUDevice & pGPUDevice,
			EShaderType pShaderType,
			std::unique_ptr<ShaderBinary> pShaderBinary = nullptr,
			resource_id_t pShaderID = CxDefs::RESOURCE_ID_INVALID );

		virtual ~Shader();
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_SHADER_H__
