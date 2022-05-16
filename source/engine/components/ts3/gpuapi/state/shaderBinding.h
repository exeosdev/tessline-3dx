
#pragma once

#ifndef __TS3_GPUAPI_SHADER_BINDING_H__
#define __TS3_GPUAPI_SHADER_BINDING_H__

#include "commonGPUStateDefs.h"
#include "../resources/shaderCommon.h"

namespace ts3::gpuapi
{

	struct GraphicsShaderBindingDesc
	{
		using ShaderList = std::array<Shader *, E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;
		ShaderList shaderList;
	};

	struct GraphicsShaderBinding
	{
		using ShaderArray = std::array<Shader *, E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;

		/// A fixed-size array with all supported shaders, indexed using EShaderStageIndex values.
		ShaderArray shaderArray;

		/// Number of active stages. Naturally, this is also the number of non-null entries in shaderArray.
		uint32 activeStagesNum;

		/// Active stage mask.
		/// @see EShaderStageFlags
		Bitmask<EShaderStageFlags> activeStagesMask;

		GraphicsShaderBinding()
		{
			reset();
		}

		TS3_GPUAPI_API void reset();

		TS3_GPUAPI_API_NO_DISCARD Shader * get( uint32 pStageIndex ) const;

		TS3_GPUAPI_API_NO_DISCARD bool isSet( uint32 pStageIndex ) const;

		TS3_GPUAPI_API_NO_DISCARD bool isValid() const;

		TS3_FUNC_NO_DISCARD Shader * get( EShaderType pShaderType ) const
		{
			return get( ecGetShaderStageIndex( pShaderType ) );
		}

		TS3_FUNC_NO_DISCARD Shader * get( EGraphicsShaderStageID pGraphicsShaderStageID ) const
		{
			return get( ecGetShaderStageIndex( pGraphicsShaderStageID ) );
		}

		TS3_FUNC_NO_DISCARD bool isSet( EShaderType pShaderType ) const
		{
			return isSet( ecGetShaderStageIndex( pShaderType ) );
		}

		TS3_FUNC_NO_DISCARD bool isSet( EGraphicsShaderStageID pGraphicsShaderStageID ) const
		{
			return isSet( ecGetShaderStageIndex( pGraphicsShaderStageID ) );
		}

		TS3_FUNC_NO_DISCARD bool empty() const
		{
			return ( activeStagesNum == 0 ) || activeStagesMask.empty();
		}

		TS3_FUNC_NO_DISCARD explicit operator bool() const
		{
			return isValid();
		}

		TS3_FUNC_NO_DISCARD Shader * operator[]( uint32 pStageIndex ) const
		{
			return get( pStageIndex );
		}

		TS3_FUNC_NO_DISCARD Shader * operator[]( EShaderType pShaderType ) const
		{
			return get( pShaderType );
		}

		TS3_FUNC_NO_DISCARD Shader * operator[]( EGraphicsShaderStageID pGraphicsShaderStageID ) const
		{
			return get( pGraphicsShaderStageID );
		}

		TS3_GPUAPI_API static GraphicsShaderBinding createFromDesc( const GraphicsShaderBindingDesc & pBindingDesc );

		TS3_GPUAPI_API static bool validateDesc( const GraphicsShaderBindingDesc & pBindingDesc );
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SHADER_BINDING_H__
