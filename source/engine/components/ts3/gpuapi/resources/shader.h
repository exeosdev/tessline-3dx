
#pragma once

#ifndef __TS3_GPUAPI_SHADER_H__
#define __TS3_GPUAPI_SHADER_H__

#include "commonGPUResourceDefs.h"
#include "shaderCommon.h"

namespace ts3::gpuapi
{

	class TS3_GPUAPI_CLASS Shader : public GPUDeviceChildObject
	{
	public:
		EShaderType const mShaderType;

		std::unique_ptr<ShaderBinary> const mShaderBinary;

	public:
		Shader( GPUDevice & pGPUDevice, EShaderType pShaderType );
		Shader( GPUDevice & pGPUDevice, EShaderType pShaderType, std::unique_ptr<ShaderBinary> pShaderBinary );
		virtual ~Shader();

//		virtual ResultCode compileFromSource( const void * pSource, size_t pSourceLength ) = 0;
//
//		virtual ResultCode compileFromSource(
//				const void * pSource,
//				size_t pSourceLength,
//				const std::string & pOptions,
//				Bitmask<uint64> pFlags ) = 0;
//
//		virtual ResultCode loadPrecompiledBinary( const void * pBinary, size_t pBinaryLength ) = 0;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SHADER_H__
