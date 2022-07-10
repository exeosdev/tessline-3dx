
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_H__
#define __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_H__

#include "graphicsPipelineDescriptor.h"
#include "../resources/shaderCommon.h"

namespace ts3::gpuapi
{

	/// @brief
	struct GraphicsShaderSet
	{
		using ShaderStageArray = std::array<ShaderHandle, E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;

		/// A fixed-size array with all supported shaders, indexed using EShaderStageIndex values.
		ShaderStageArray shaderStages{};

		/// Number of active stages. Naturally, this is also the number of non-null entries in shaderArray.
		uint32 activeStagesNum = 0;

		/// Active stage mask.
		/// @see EShaderStageFlags
		Bitmask<EShaderStageFlags> activeStagesMask = 0;

		TS3_FUNC_NO_DISCARD ShaderHandle & operator[]( EGraphicsShaderStageID pStageID ) noexcept
		{
			return shaderStages[ecGetShaderStageIndex( pStageID )];
		}

		TS3_FUNC_NO_DISCARD const ShaderHandle & operator[]( EGraphicsShaderStageID pStageID ) const noexcept
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

	struct GraphicsShaderBindingDescriptorCreateInfo
	{
		GraphicsShaderSet * shaderSet = nullptr;

		Bitmask<EShaderStageFlags> activeStagesMask = E_SHADER_STAGE_MASK_GRAPHICS_ALL;

		ArrayView<ShaderHandle> shaderList;
	};

	class GraphicsShaderBindingDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		Bitmask<EShaderStageFlags> const mActiveStagesMask;

		uint32 const mActiveStagesNum;

	public:
		GraphicsShaderBindingDescriptor( GPUDevice & pGPUDevice,
		                                 pipeline_descriptor_id_t pDescriptorID,
		                                 Bitmask<EShaderStageFlags> pActiveStagesMask,
		                                 uint32 pActiveStagesNum );

		virtual ~GraphicsShaderBindingDescriptor();
	};

	namespace smu
	{

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD GraphicsShaderSet createGraphicsShaderSet(
				const ArrayView<ShaderHandle> & pShaderList,
				Bitmask<EShaderStageFlags> pStagesMask = E_GRAPHICS_SHADER_STAGE_MASK_GRAPHICS_ALL ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD bool validateGraphicsShaderSet( const GraphicsShaderSet & pShaderSet ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_H__
