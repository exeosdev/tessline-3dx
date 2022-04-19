
#include "shaderBinding.h"
#include "../resources/shader.h"
#include <ts3/stdext/memory.h>

namespace ts3
{
namespace gpuapi
{

	struct ShaderBindingInfo
	{
		using ShaderArray = std::array<Shader *, GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;
		ShaderArray shaderArray;
	};

	bool validateGraphicsShaderBindingDesc( const GraphicsShaderBindingDesc & pShaderBindingDesc, ShaderBindingInfo & pBindingInfo );

	GraphicsShaderBinding createGraphicsShaderBinding( const GraphicsShaderBindingDesc & pBindingDesc )
	{
		ShaderBindingInfo bindingInfo;
		if( !validateGraphicsShaderBindingDesc( pBindingDesc, bindingInfo ) )
		{
			return cvInitEmpty;
		}

		GraphicsShaderBinding graphicsShaderBinding;
		graphicsShaderBinding.activeStagesNum = 0;
		graphicsShaderBinding.activeStagesMask = 0;
		memZero( graphicsShaderBinding.bindingArray );

		for( uint32 stageIndex = 0; stageIndex < GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM; ++stageIndex )
		{
			if( auto * shader = bindingInfo.shaderArray[stageIndex] )
			{
				auto graphicsStageID = ecGetEGraphicsShaderStageIDByIndex( stageIndex );
				auto graphicsStageMaskBit = ecGetShaderStageMaskBit( graphicsStageID );
				graphicsShaderBinding.bindingArray[stageIndex] = shader;
				graphicsShaderBinding.activeStagesNum += 1;
				graphicsShaderBinding.activeStageList.push_back( { graphicsStageID, shader } );
				graphicsShaderBinding.activeStagesMask.set( graphicsStageMaskBit );
			}
		}

		ts3DebugAssert( graphicsShaderBinding.activeStagesNum > 0 );
		ts3DebugAssert( graphicsShaderBinding.activeStagesMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT ) );

		return graphicsShaderBinding;
	}

	bool validateGraphicsShaderBindingDesc( const GraphicsShaderBindingDesc & pShaderBindingDesc, ShaderBindingInfo & pBindingInfo )
	{
		memZero( pBindingInfo.shaderArray );

		for( const auto & stageDesc : pShaderBindingDesc.shaderStageDescArray )
		{
			if( !stageDesc.shaderObject )
			{
				continue;
			}

			auto shaderObjectStageID = ecGetEGraphicsShaderStageID( stageDesc.shaderObject->mEShaderType );
			if( shaderObjectStageID != stageDesc.stageID )
			{
				ts3DebugInterrupt();
				continue;
			}

			auto stageIndex = ecGetEShaderStageIndex( shaderObjectStageID );
			pBindingInfo.shaderArray[stageIndex] = stageDesc.shaderObject;
		}

		if( !pBindingInfo.shaderArray[E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX] )
		{
			return false;
		}

		return true;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
