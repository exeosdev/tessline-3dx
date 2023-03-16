
#include "shaderLibrary.h"

#include <ts3/gpuapi/gpuUtils.h>

namespace ts3
{

	ShaderLibrary::ShaderLibrary( gpuapi::GPUDeviceHandle pGPUDevice )
	: _gpuDevice( std::move( pGPUDevice ) )
	{}

	ShaderLibrary::~ShaderLibrary() = default;

	gpuapi::GPUDevice & ShaderLibrary::gpuDevice() const noexcept
	{
		return *_gpuDevice;
	}

	gpuapi::ShaderHandle ShaderLibrary::getShader( const ShaderID & pShaderID ) const noexcept
	{
		const auto shaderRef = _shaderMap.find( pShaderID );
		return ( shaderRef != _shaderMap.end() ) ? shaderRef->second : nullptr;
	}

	void ShaderLibrary::loadShaders( std::initializer_list<ShaderSourceDefinition> pShaderDefinitions )
	{
		if( pShaderDefinitions.size() == 0 )
		{
			return;
		}

		for( auto & shaderDefinition : pShaderDefinitions )
		{
			auto shaderSource = shaderDefinition.sourceLoadCallback();
			ts3DebugOutput( shaderSource.data() );
			if( !shaderSource.empty() )
			{
				auto shaderObject = gpuapi::utils::createShaderFromSource(
						*_gpuDevice,
						shaderDefinition.shaderType,
						shaderSource.data(),
						shaderSource.size() );

				if( shaderObject )
				{
					registerShader( shaderDefinition.shaderID, shaderObject );
				}
			}
		}
	}

	void ShaderLibrary::registerShader( ShaderID pShaderID, gpuapi::ShaderHandle pShader )
	{
		_shaderMap[pShaderID] = std::move( pShader );
	}

}
