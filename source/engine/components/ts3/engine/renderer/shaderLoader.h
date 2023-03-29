
#pragma once

#ifndef __TS3_ENGINE_SHADER_LOADER_H__
#define __TS3_ENGINE_SHADER_LOADER_H__

#include "commonRendererDefs.h"
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <ts3/system/assetCommon.h>

namespace ts3
{

	using ShaderFileLoadCallback = std::function<DynamicMemoryBuffer( const std::string & )>;
	using ShaderInputReadCallback = std::function<DynamicMemoryBuffer()>;

	struct ShaderFileDefinition
	{
		std::string shaderName;
		gpuapi::EShaderType shaderType;
		std::string shaderFileName;
	};

	struct ShaderInputDefinition
	{
		std::string shaderName;
		gpuapi::EShaderType shaderType;
		ShaderInputReadCallback inputReadCallback;
	};

	class ShaderLoader : public CoreEngineObject
	{
	public:
		explicit ShaderLoader( const CoreEngineState & pCES );

		virtual ~ShaderLoader();

		gpuapi::ShaderHandle loadShader( const ShaderInputDefinition & pShaderDefinition );

		ShaderLibraryHandle loadShaders( std::initializer_list<ShaderInputDefinition> pShaderDefinitions );

		uint32 loadShaders(
				ShaderLibrary & pShaderLibrary,
				std::initializer_list<ShaderInputDefinition> pShaderDefinitions );

		static ShaderLibraryHandle createShaderLibrary(
				const CoreEngineState & pCES,
				std::initializer_list<ShaderInputDefinition> pShaderDefinitions );
	};

	class FileShaderLoader : public ShaderLoader
	{
	public:
		FileShaderLoader(
				const CoreEngineState & pCES,
				ShaderFileLoadCallback pFileLoadCallback );

		FileShaderLoader(
				const CoreEngineState & pCES,
				system::AssetLoaderHandle pAssetLoader,
				std::string pShaderBaseSubDirectory );

		virtual ~FileShaderLoader();

		gpuapi::ShaderHandle loadShader( const ShaderFileDefinition & pShaderDefinition );

		ShaderLibraryHandle loadShaders( std::initializer_list<ShaderFileDefinition> pShaderDefinitions );

		uint32 loadShaders(
				ShaderLibrary & pShaderLibrary,
				std::initializer_list<ShaderFileDefinition> pShaderDefinitions );

		static ShaderLibraryHandle createShaderLibrary(
				const CoreEngineState & pCES,
				ShaderFileLoadCallback pFileLoadCallback,
				std::initializer_list<ShaderFileDefinition> pShaderDefinitions );

		static ShaderLibraryHandle createShaderLibrary(
				const CoreEngineState & pCES,
				system::AssetLoaderHandle pAssetLoader,
				std::string pShaderBaseSubDirectory,
				std::initializer_list<ShaderFileDefinition> pShaderDefinitions );

	private:
		static ShaderFileLoadCallback bindShaderFileLoadCallback(
				system::AssetLoaderHandle pAssetLoader,
				std::string pShaderBaseSubDirectory );

		static DynamicMemoryBuffer loadShaderFile( system::AssetLoader & pAssetLoader, const std::string & pFilename );

	private:
		ShaderFileLoadCallback _fileLoadCallback;
	};

} // namespace ts3

#endif // __TS3_ENGINE_SHADER_LOADER_H__
