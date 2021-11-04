
#pragma once

#ifndef __TS3_GPUAPI_SHADER_BINDING_H__
#define __TS3_GPUAPI_SHADER_BINDING_H__

#include "commonGPUStateDefs.h"
#include "../resources/shaderCommon.h"

namespace ts3
{
namespace gpuapi
{

	struct GraphicsShaderBindingDesc
	{
		struct ShaderStageDesc
		{
			EGraphicsShaderStageID stageID = EGraphicsShaderStageID::Unknown;
			Shader * shaderObject = nullptr;
		};
		using ShaderStageDescArray = std::array<ShaderStageDesc, GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;
		ShaderStageDescArray shaderStageDescArray;
	};

	struct GraphicsShaderBinding
	{
		struct ShaderStage
		{
			EGraphicsShaderStageID stageID;
			Shader * shaderObject;
		};
		using ActiveStageList = std::list<ShaderStage>;
		using BindingArray = std::array<Shader *, GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;
		ActiveStageList activeStageList;
		BindingArray bindingArray;
		Bitmask<EShaderStageFlags> activeStagesMask;
		uint32 activeStagesNum;

		GraphicsShaderBinding() = default;

		GraphicsShaderBinding( const tag_init_empty_t & )
		: activeStagesMask( 0 )
		{}

		explicit operator bool() const
		{
			return activeStagesMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT );
		}

		Shader * operator[]( size_t pStageIndex ) const
		{
			return bindingArray.at( pStageIndex );
		}

		Shader * operator[]( EShaderType pShaderType ) const
		{
			auto shaderStageIndex = ecGetEShaderStageIndex( pShaderType );
			return bindingArray.at( shaderStageIndex );
		}

		Shader * operator[]( EGraphicsShaderStageID pGraphicsShaderStageID ) const
		{
			auto shaderStageIndex = ecGetEShaderStageIndex( pGraphicsShaderStageID );
			return bindingArray.at( shaderStageIndex );
		}
	};

	TS3_GPUAPI_API GraphicsShaderBinding createGraphicsShaderBinding( const GraphicsShaderBindingDesc & pBindingDesc );

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_SHADER_BINDING_H__
