
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_SHADER_LINKAGE_H__
#define __TS3_GPUAPI_GRAPHICS_SHADER_LINKAGE_H__

#include "gpuStateObject.h"
#include "graphicsPipelineDescriptor.h"
#include "../resources/shaderCommon.h"

namespace ts3::GpuAPI
{

	using ShaderObjectArray = std::array<Shader *, E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;

	struct GraphicsShaderLinkageDescriptorCreateInfo
	{
		ShaderObjectArray shaderArray;
	};

	class GraphicsShaderLinkage
	{
	public:
		/// Active stages mask. Contains all active stages which are part of this linked set.
		Bitmask<EShaderStageFlags> const mActiveStagesMask;

		/// Number of active stages in the linked set.
		/// This does not necessarily matches the number of shader objects (e.g. PS4/PS5).
		uint32 const mActiveStagesNum;

	public:
		GraphicsShaderLinkageDescriptor(
			GPUDevice & pGPUDevice,
			PipelineDescriptorID pDescriptorID,
			Bitmask<EShaderStageFlags> pActiveStagesMask );

		virtual ~GraphicsShaderLinkageDescriptor();
	};

	// State Management Utility API
	namespace StateMgmt
	{

		/// @brief Returns
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EShaderStageFlags> getActiveShaderStagesMask(
			const ShaderObjectArray & pShaders ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD uint32 getActiveShaderStagesNum( const ShaderObjectArray & pShaders ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD PipelineDescriptorHash computePipelineDescriptorHash(
			const GraphicsShaderLinkageDescriptorCreateInfo & pCreateInfo ) noexcept;

	}

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_GRAPHICS_SHADER_LINKAGE_H__
