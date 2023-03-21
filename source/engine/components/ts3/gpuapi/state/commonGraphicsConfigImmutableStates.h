
#pragma once

#ifndef __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_IMMUTABLE_STATES_H__
#define __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_IMMUTABLE_STATES_H__

#include "commonGraphicsConfig.h"
#include "graphicsPipelineImmutableState.h"

namespace ts3::gpuapi
{

	/// @brief
	class BlendImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		Bitmask<EBlendConfigFlags> const mBlendFlags;

	public:
		BlendImmutableState( const BlendImmutableState & ) = delete;
		BlendImmutableState & operator=( const BlendImmutableState & ) = delete;

		BlendImmutableState( GPUDevice & pGPUDevice, Bitmask<EBlendConfigFlags> pBlendFlags );
		virtual ~BlendImmutableState();
	};

	/// @brief
	class DepthStencilImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		Bitmask<EDepthStencilConfigFlags> mDepthStencilFlags;

	public:
		DepthStencilImmutableState( const DepthStencilImmutableState & ) = delete;
		DepthStencilImmutableState & operator=( const DepthStencilImmutableState & ) = delete;

		DepthStencilImmutableState( GPUDevice & pGPUDevice, Bitmask<EDepthStencilConfigFlags> pDepthStencilFlags );
		virtual ~DepthStencilImmutableState();
	};

	/// @brief
	class RasterizerImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		Bitmask<ERasterizerConfigFlags> const mRasterizerFlags;

	public:
		RasterizerImmutableState( const RasterizerImmutableState & ) = delete;
		RasterizerImmutableState & operator=( const RasterizerImmutableState & ) = delete;

		RasterizerImmutableState( GPUDevice & pGPUDevice, Bitmask<ERasterizerConfigFlags> pRasterizerFlags );
		virtual ~RasterizerImmutableState();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_IMMUTABLE_STATES_H__
