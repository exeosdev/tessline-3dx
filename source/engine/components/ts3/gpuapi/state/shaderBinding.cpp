
#include "shaderBinding.h"
#include "../resources/shader.h"
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	void GraphicsShaderBinding::reset()
	{
		shaderArray.fill( nullptr );
		activeStagesNum = 0;
		activeStagesMask = 0;
	}

	Shader * GraphicsShaderBinding::get( uint32 pStageIndex ) const
	{
		const auto stageIndex = ecGetShaderStageIndexFromValue( pStageIndex );
		return ( stageIndex != E_SHADER_STAGE_INDEX_INVALID ) ? shaderArray[pStageIndex] : nullptr;
	}

	bool GraphicsShaderBinding::isSet( uint32 pStageIndex ) const
	{
		const auto stageFlagBit = ecMakeGraphicsShaderStageBit( pStageIndex );
		return ( stageFlagBit != 0 ) && activeStagesMask.isSet( stageFlagBit ) && shaderArray[pStageIndex];
	}

	bool GraphicsShaderBinding::isValid() const
	{
		return ( isSet( EShaderType::GSVertex ) || isSet( EShaderType::GSMesh ) ) && isSet( EShaderType::GSPixel );
	}

	GraphicsShaderBinding GraphicsShaderBinding::createFromDesc( const GraphicsShaderBindingDesc & pBindingDesc )
	{
		GraphicsShaderBinding shaderBinding{};

		if( validateDesc( pBindingDesc ) )
		{
			for( auto * shaderObject : pBindingDesc.shaderList )
			{
				const auto graphicsStageID = ecShaderTypeToGraphicsShaderStageID( shaderObject->mShaderType );

				if( graphicsStageID != EGraphicsShaderStageID::Unknown )
				{
					const auto graphicsStageIndex = ecGetShaderStageIndex( shaderObject->mShaderType );
					const auto graphicsStageMaskBit = ecGetShaderStageBit( shaderObject->mShaderType );

					shaderBinding.shaderArray[graphicsStageIndex] = shaderObject;

					if( shaderBinding.shaderArray[graphicsStageIndex] )
					{
						ts3DebugOutput( "Shader binding overwrite (two or more shaders of the same type on the list)" );
						continue;
					}

					shaderBinding.activeStagesNum += 1;
					shaderBinding.activeStagesMask.set( graphicsStageMaskBit );
				}
			}
		}

		return shaderBinding;
	}

	bool GraphicsShaderBinding::validateDesc( const GraphicsShaderBindingDesc & pBindingDesc )
	{
		bool bindingHasVertexShader = false;
		bool bindingHasMeshShader = false;
		bool bindingHasPixelShader = false;

		for( auto * shaderObject : pBindingDesc.shaderList )
		{
			if( !shaderObject || ( shaderObject->mShaderType == EShaderType::Unknown ) )
			{
				continue;
			}

			auto shaderObjectStageID = ecShaderTypeToGraphicsShaderStageID( shaderObject->mShaderType );

			if( shaderObjectStageID == EGraphicsShaderStageID::Vertex )
			{
				bindingHasVertexShader = true;
			}
			else if( shaderObjectStageID == EGraphicsShaderStageID::Mesh )
			{
				bindingHasMeshShader = true;
			}
			else if( shaderObjectStageID == EGraphicsShaderStageID::Pixel )
			{
				bindingHasPixelShader = true;
			}
		}

		return ( bindingHasVertexShader || bindingHasMeshShader ) && bindingHasPixelShader;
	}

} // namespace ts3::gpuapi
