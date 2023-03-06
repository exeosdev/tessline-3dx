
#pragma once

#ifndef __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_IMMUTABLE_STATES_H__
#define __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_IMMUTABLE_STATES_H__

#include "../commonGraphicsConfig.h"
#include "../graphicsPipelineImmutableState.h"

namespace ts3::gpuapi
{

	/// @brief
	class BlendImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		BlendImmutableState( const BlendImmutableState & ) = delete;
		BlendImmutableState & operator=( const BlendImmutableState & ) = delete;

		BlendImmutableState( GPUDevice & pGPUDevice );
		virtual ~BlendImmutableState() = default;
	};

	/// @brief
	class DepthStencilImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		DepthStencilImmutableState( const DepthStencilImmutableState & ) = delete;
		DepthStencilImmutableState & operator=( const DepthStencilImmutableState & ) = delete;

		DepthStencilImmutableState( GPUDevice & pGPUDevice );
		virtual ~DepthStencilImmutableState() = default;
	};

	/// @brief
	class RasterizerImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		RasterizerImmutableState( const RasterizerImmutableState & ) = delete;
		RasterizerImmutableState & operator=( const RasterizerImmutableState & ) = delete;

		RasterizerImmutableState( GPUDevice & pGPUDevice );
		virtual ~RasterizerImmutableState() = default;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_IMMUTABLE_STATES_H__
