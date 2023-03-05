
#include "graphicsShaderState.h"
#include <ts3/gpuapi/resources/shader.h>

namespace ts3::gpuapi
{

	GraphicsShaderSet::GraphicsShaderSet()
	: commonShaderArray( _shaderArray )
	, vertexShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX] )
	, hullShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_HULL] )
	, domainShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_DOMAIN] )
	, geometryShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY] )
	, pixelShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL] )
	{}

	GraphicsShaderSet::GraphicsShaderSet( const GraphicsShaderSet & pSource )
	: _shaderArray( pSource._shaderArray )
	, commonShaderArray( _shaderArray )
	, vertexShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX] )
	, hullShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_HULL] )
	, domainShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_DOMAIN] )
	, geometryShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY] )
	, pixelShader( _shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL] )
	{}

	GraphicsShaderSet::GraphicsShaderSet( const GraphicsShaderArray & pShaderArray )
	: GraphicsShaderSet()
	{
		setShaders( pShaderArray );
	}

	GraphicsShaderSet & GraphicsShaderSet::operator=( const GraphicsShaderSet & pRhs )
	{
		_shaderArray = pRhs._shaderArray;
		return *this;
	}

	GraphicsShaderSet & GraphicsShaderSet::operator=( const GraphicsShaderArray & pRhs )
	{
		setShaders( pRhs );
		return *this;
	}

	const GraphicsShaderSet::ShaderRefType & GraphicsShaderSet::operator[]( size_t pIndex ) const noexcept
	{
		ts3DebugAssert( cxdefs::isShaderStageIndexValid( pIndex ) );
		return commonShaderArray[pIndex];
	}

	const GraphicsShaderSet::ShaderRefType & GraphicsShaderSet::operator[]( EShaderType pShaderType ) const noexcept
	{
		ts3DebugAssert( cxdefs::isShaderTypeGraphics( pShaderType ) );
		const auto stageIndex = cxdefs::getShaderStageIndex( pShaderType );
		return commonShaderArray[stageIndex];
	}

	Bitmask<EShaderStageFlags> GraphicsShaderSet::getActiveShaderStagesMask() const noexcept
	{
		return smutil::getActiveShaderStagesMask( commonShaderArray );
	}

	uint32 GraphicsShaderSet::getActiveShaderStagesNum() const noexcept
	{
		return smutil::getActiveShaderStagesNum( commonShaderArray );
	}

	bool GraphicsShaderSet::validateShaders() const noexcept
	{
		for( uint32 stageIndex = 0; stageIndex < cxdefs::GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM; ++stageIndex )
		{
			if( auto shader = _shaderArray[stageIndex] )
			{
				const auto shaderType = cxdefs::getShaderTypeFromStageIndex( stageIndex );
				if( shader->mShaderType != shaderType )
				{
					return false;
				}
			}
		}

		return true;
	}

	void GraphicsShaderSet::addShader( ShaderHandle pShader ) noexcept
	{
		if( pShader )
		{
			const auto stageIndex = cxdefs::getShaderStageIndex( pShader->mShaderType );
			_shaderArray[stageIndex] = pShader;
		}
	}

	void GraphicsShaderSet::setShaders( const GraphicsShaderArray & pShaderArray ) noexcept
	{
		for( uint32 stageIndex = 0; stageIndex < cxdefs::GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM; ++stageIndex )
		{
			if( const auto shader = pShaderArray[stageIndex] )
			{
				const auto shaderTypeAtIndex = cxdefs::getShaderTypeFromStageIndex( stageIndex );
				if( shader->mShaderType == shaderTypeAtIndex )
				{
					_shaderArray[stageIndex] = shader;
				}
			}
		}
	}

	void GraphicsShaderSet::resetStage( uint32 pStageIndex ) noexcept
	{
		ts3DebugAssert( cxdefs::isShaderStageIndexValidGraphics( pStageIndex ) );
		_shaderArray[pStageIndex] = cvNullHandle;
	}

	void GraphicsShaderSet::resetStage( EShaderType pShaderType ) noexcept
	{
		ts3DebugAssert( cxdefs::isShaderTypeGraphics( pShaderType ) );
		const auto stageIndex = cxdefs::getShaderStageIndex( pShaderType );
		_shaderArray[stageIndex] = cvNullHandle;
	}

	namespace smutil
	{

		Bitmask<EShaderStageFlags> getActiveShaderStagesMask(
				const GraphicsShaderArray & pShaderArray ) noexcept
		{
			Bitmask<EShaderStageFlags> activeStagesMask = 0;

			for( uint32 stageIndex = 0; stageIndex < cxdefs::GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM; ++stageIndex )
			{
				if( const auto shader = pShaderArray[stageIndex] )
				{
					const auto shaderTypeAtIndex = cxdefs::getShaderTypeFromStageIndex( stageIndex );
					if( shader->mShaderType == shaderTypeAtIndex )
					{
						const auto shaderTypeBit = cxdefs::makeShaderStageBit( stageIndex );
						activeStagesMask.set( shaderTypeBit );
					}
				}
			}

			return activeStagesMask;
		}

		uint32 getActiveShaderStagesNum( const GraphicsShaderArray & pShaderArray ) noexcept
		{
			const auto activeStagesMask = getActiveShaderStagesMask( pShaderArray );
			const auto activeStagesNum = popCount( activeStagesMask );
			return activeStagesNum;
		}

	}

}
