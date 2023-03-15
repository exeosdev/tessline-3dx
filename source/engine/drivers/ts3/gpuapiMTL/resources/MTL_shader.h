
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_SHADER_INTERFACE_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_SHADER_INTERFACE_H__

#include "../MTL_prerequisites.h"
#include <ts3/gpuapi/resources/shader.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( MetalShader );

	class TS3_GPUAPI_CLASS MetalShader : public Shader
	{
	public:
		MetalShaderObjectHandle const mMetalShaderObject;
		MetalShaderProgramObjectHandle const mMetalShaderProgramObject;

	public:
		MetalShader( MetalGPUDevice & pGPUDevice, EShaderType pShaderType, MetalShaderObjectHandle pMetalShaderObject );
		MetalShader( MetalGPUDevice & pGPUDevice, EShaderType pShaderType, MetalShaderProgramObjectHandle pMetalShaderProgramObject );
		virtual ~MetalShader();

		static MetalShaderHandle createShaderObjectWithBinary( MetalGPUDevice & pGPUDevice, EShaderType pShaderType, const ShaderBinary & pShaderBinary );
		static MetalShaderHandle createShaderObjectWithSource( MetalGPUDevice & pGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength );
		static MetalShaderHandle createShaderSeparableStageWithBinary( MetalGPUDevice & pGPUDevice, EShaderType pShaderType, const ShaderBinary & pShaderBinary );
		static MetalShaderHandle createShaderSeparableStageWithSource( MetalGPUDevice & pGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength );
	};

	namespace rcutil
	{

		MetalShaderHandle createShaderObject( MetalGPUDevice & pGPUDevice, const ShaderCreateInfo & pCreateInfo );

		MetalShaderHandle createShaderSeparableStage( MetalGPUDevice & pGPUDevice, const ShaderCreateInfo & pCreateInfo );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_SHADER_INTERFACE_H__
