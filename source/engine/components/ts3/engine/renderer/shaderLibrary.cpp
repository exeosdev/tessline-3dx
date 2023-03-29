
#include "shaderLibrary.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/gpuUtils.h>

namespace ts3
{

	ShaderLibrary::ShaderLibrary( const CoreEngineState & pCES )
	: CoreEngineObject( pCES )
	{}

	ShaderLibrary::~ShaderLibrary() = default;

	gpuapi::ShaderHandle ShaderLibrary::getShader( GpaUniqueObjectID pShaderID ) const noexcept
	{
		const auto shaderIter = _shaderMap.find( pShaderID );
		return ( shaderIter != _shaderMap.end() ) ? shaderIter->second : nullptr;
	}

	gpuapi::ShaderHandle ShaderLibrary::getShader( const GpaUniqueObjectName & pShaderName ) const noexcept
	{
		const auto uniqueShaderID = gpuapi::generateUniqueGPUObjectID( pShaderName );
		return getShader( uniqueShaderID );
	}

	uint32 ShaderLibrary::append( const ShaderLibrary & pOtherLibrary )
	{
		uint32 addedShadersNum = 0;

		for( const auto & shaderDef : pOtherLibrary._shaderMap )
		{
			if( registerShader( shaderDef.first, shaderDef.second ) )
			{
				++addedShadersNum;
			}
		}

		return addedShadersNum;
	}

	bool ShaderLibrary::registerShader( GpaUniqueObjectID pShaderID, gpuapi::ShaderHandle pShaderObject )
	{
		if( !pShaderID || !pShaderObject )
		{
			ts3DebugOutput( "Cannot register shader: empty name or shader handle" );
			return false;
		}

		auto existingShaderIter = _shaderMap.find( pShaderID );
		if( existingShaderIter != _shaderMap.end() )
		{
			return false;
		}

		_shaderMap.insert( { pShaderID, pShaderObject } );

		return true;
	}

	bool ShaderLibrary::registerShader( const GpaUniqueObjectName & pShaderName, gpuapi::ShaderHandle pShaderObject )
	{
		const auto uniqueShaderID = gpuapi::generateUniqueGPUObjectID( pShaderName );
		return registerShader( uniqueShaderID, pShaderObject );
	}

}
