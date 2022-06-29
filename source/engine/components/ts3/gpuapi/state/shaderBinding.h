
#pragma once

#ifndef __TS3_GPUAPI_SHADER_BINDING_H__
#define __TS3_GPUAPI_SHADER_BINDING_H__

#include "commonGPUStateDefs.h"
#include "../resources/shaderCommon.h"

namespace ts3::gpuapi
{

	struct GraphicsShaderSet
	{
		using ShaderStageArray = std::array<Shader *, E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;

		/// A fixed-size array with all supported shaders, indexed using EShaderStageIndex values.
		ShaderStageArray shaderStages{};

		/// Number of active stages. Naturally, this is also the number of non-null entries in shaderArray.
		uint32 activeStagesNum = 0;

		/// Active stage mask.
		/// @see EShaderStageFlags
		Bitmask<EShaderStageFlags> activeStagesMask = 0;

		TS3_FUNC_NO_DISCARD Shader * & operator[]( EGraphicsShaderStageID pStageID ) noexcept
		{
			return shaderStages[ecGetShaderStageIndex( pStageID )];
		}

		TS3_FUNC_NO_DISCARD Shader * const & operator[]( EGraphicsShaderStageID pStageID ) const noexcept
		{
			return shaderStages[ecGetShaderStageIndex( pStageID )];
		}

		TS3_FUNC_NO_DISCARD bool isEmpty() const
		{
			return activeStagesNum == 0;
		}

		TS3_FUNC_NO_DISCARD explicit operator bool() const noexcept
		{
			return !isEmpty();
		}
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SHADER_BINDING_H__
