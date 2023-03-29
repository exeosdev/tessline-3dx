
#include "shaderLoader.h"
#include "shaderLibrary.h"
#include <ts3/gpuapi/gpuUtils.h>
#include <ts3/system/assetSystem.h>

namespace ts3
{

	ShaderLoader::ShaderLoader( const CoreEngineState & pCES )
	: CoreEngineObject( pCES )
	{}

	ShaderLoader::~ShaderLoader() = default;

	gpuapi::ShaderHandle ShaderLoader::loadShader( const ShaderInputDefinition & pShaderDefinition )
	{
		gpuapi::ShaderHandle shaderObject = nullptr;

		auto shaderSource = pShaderDefinition.inputReadCallback();
		if( !shaderSource.empty() )
		{
			shaderObject = gpuapi::utils::createShaderFromSource(
					*mCES.mGPUDevice,
					pShaderDefinition.shaderType,
					shaderSource.data(),
					shaderSource.size() );
		}

		return shaderObject;
	}

	ShaderLibraryHandle ShaderLoader::loadShaders( std::initializer_list<ShaderInputDefinition> pShaderDefinitions )
	{
		ShaderLibraryHandle shaderLibrary = nullptr;

		if( pShaderDefinitions.size() > 0 )
		{
			shaderLibrary = createDynamicInterfaceObject<ShaderLibrary>( mCES );

			for( auto & shaderDefinition : pShaderDefinitions )
			{
				if( auto shaderObject = loadShader( shaderDefinition ) )
				{
					shaderLibrary->registerShader( shaderDefinition.shaderName, shaderObject );
				}
			}

			if( shaderLibrary->empty() )
			{
				shaderLibrary = nullptr;
			}
		}

		return shaderLibrary;
	}

	uint32 ShaderLoader::loadShaders(
			ShaderLibrary & pShaderLibrary,
			std::initializer_list<ShaderInputDefinition> pShaderDefinitions )
	{
		uint32 loadedShadersNum = 0;

		if( pShaderDefinitions.size() > 0 )
		{
			for( auto & shaderDefinition : pShaderDefinitions )
			{
				if( auto shaderObject = loadShader( shaderDefinition ) )
				{
					pShaderLibrary.registerShader( shaderDefinition.shaderName, shaderObject );
					loadedShadersNum += 1;
				}
			}
		}

		return loadedShadersNum;
	}

	ShaderLibraryHandle ShaderLoader::createShaderLibrary(
			const CoreEngineState & pCES,
			std::initializer_list<ShaderInputDefinition> pShaderDefinitions )
	{
		return ShaderLoader( pCES ).loadShaders( pShaderDefinitions );
	}


	FileShaderLoader::FileShaderLoader(
			const CoreEngineState & pCES,
			ShaderFileLoadCallback pFileLoadCallback)
	: ShaderLoader( pCES )
	{}

	FileShaderLoader::FileShaderLoader(
			const CoreEngineState & pCES,
			system::AssetLoaderHandle pAssetLoader,
			std::string pShaderBaseSubDirectory )
	: FileShaderLoader( pCES, bindShaderFileLoadCallback( pAssetLoader, std::move( pShaderBaseSubDirectory ) ) )
	{}

	FileShaderLoader::~FileShaderLoader() = default;

	gpuapi::ShaderHandle FileShaderLoader::loadShader( const ShaderFileDefinition & pShaderDefinition )
	{
		gpuapi::ShaderHandle shaderObject = nullptr;

		auto shaderSource = _fileLoadCallback( pShaderDefinition.shaderFileName );
		if( !shaderSource.empty() )
		{
			shaderObject = gpuapi::utils::createShaderFromSource(
					*mCES.mGPUDevice,
					pShaderDefinition.shaderType,
					shaderSource.data(),
					shaderSource.size() );
		}

		return shaderObject;
	}

	ShaderLibraryHandle FileShaderLoader::loadShaders( std::initializer_list<ShaderFileDefinition> pShaderDefinitions )
	{
		ShaderLibraryHandle shaderLibrary = nullptr;

		if( pShaderDefinitions.size() > 0 )
		{
			shaderLibrary = createDynamicInterfaceObject<ShaderLibrary>( mCES );

			for( auto & shaderDefinition : pShaderDefinitions )
			{
				if( auto shaderObject = loadShader( shaderDefinition ) )
				{
					shaderLibrary->registerShader( shaderDefinition.shaderName, shaderObject );
				}
			}

			if( shaderLibrary->empty() )
			{
				shaderLibrary = nullptr;
			}
		}

		return shaderLibrary;
	}

	uint32 FileShaderLoader::loadShaders(
			ShaderLibrary & pShaderLibrary,
			std::initializer_list<ShaderFileDefinition> pShaderDefinitions )
	{
		uint32 loadedShadersNum = 0;

		if( pShaderDefinitions.size() > 0 )
		{
			for( auto & shaderDefinition : pShaderDefinitions )
			{
				if( auto shaderObject = loadShader( shaderDefinition ) )
				{
					pShaderLibrary.registerShader( shaderDefinition.shaderName, shaderObject );
					loadedShadersNum += 1;
				}
			}
		}

		return loadedShadersNum;
	}

	ShaderLibraryHandle FileShaderLoader::createShaderLibrary(
			const CoreEngineState & pCES,
			ShaderFileLoadCallback pFileLoadCallback,
			std::initializer_list<ShaderFileDefinition> pShaderDefinitions )
	{
		return FileShaderLoader( pCES, std::move( pFileLoadCallback ) ).loadShaders( pShaderDefinitions );
	}

	ShaderLibraryHandle FileShaderLoader::createShaderLibrary(
			const CoreEngineState & pCES,
			system::AssetLoaderHandle pAssetLoader,
			std::string pShaderBaseSubDirectory,
			std::initializer_list<ShaderFileDefinition> pShaderDefinitions )
	{
		return FileShaderLoader( pCES, pAssetLoader, std::move( pShaderBaseSubDirectory ) ).loadShaders( pShaderDefinitions );
	}

	ShaderFileLoadCallback FileShaderLoader::bindShaderFileLoadCallback( system::AssetLoaderHandle pAssetLoader, std::string pShaderBaseSubDirectory )
	{
		return [pAssetLoader, pSubDir = std::move( pShaderBaseSubDirectory )]( const std::string & pFilename ){
			return loadShaderFile( *pAssetLoader, pSubDir + "/" + pFilename );
		};
	}

	DynamicMemoryBuffer FileShaderLoader::loadShaderFile( system::AssetLoader & pAssetLoader, const std::string & pFilename )
	{
		auto psAsset = pAssetLoader.openSubAsset(
				pFilename,
				system::E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );

		DynamicMemoryBuffer resultBuffer;
		const auto sourceLength = psAsset->readAll( resultBuffer, 1 );
		resultBuffer[sourceLength] = 0;

		return resultBuffer;
	}

}
