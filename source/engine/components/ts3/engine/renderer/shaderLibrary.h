
#pragma once

#ifndef __TS3_ENGINE_SHADER_LIBRARY_H__
#define __TS3_ENGINE_SHADER_LIBRARY_H__

#include "../prerequisites.h"
#include <ts3/gpuapi/resources/shaderCommon.h>

#include <unordered_map>

namespace ts3
{

	using ShaderID = std::string;

	using ShaderSourceLoadCallback = std::function<DynamicMemoryBuffer()>;

	struct ShaderSourceDefinition
	{
		ShaderID shaderID;
		gpuapi::EShaderType shaderType;
		ShaderSourceLoadCallback sourceLoadCallback;
	};

	class ShaderLibrary
	{
	public:
		explicit ShaderLibrary( gpuapi::GPUDeviceHandle pGPUDevice );
		~ShaderLibrary();

		TS3_ATTR_NO_DISCARD gpuapi::GPUDevice & gpuDevice() const noexcept;

		TS3_ATTR_NO_DISCARD gpuapi::ShaderHandle getShader( const ShaderID & pShaderID ) const noexcept;

		void loadShaders( std::initializer_list<ShaderSourceDefinition> pShaderDefinitions );

		void registerShader( ShaderID pShaderID, gpuapi::ShaderHandle pShader );

	private:
		using ShaderMap = std::unordered_map<ShaderID, gpuapi::ShaderHandle>;
		gpuapi::GPUDeviceHandle _gpuDevice;
		ShaderMap _shaderMap;
	};

} // namespace ts3

#endif // __TS3_ENGINE_SHADER_LIBRARY_H__
