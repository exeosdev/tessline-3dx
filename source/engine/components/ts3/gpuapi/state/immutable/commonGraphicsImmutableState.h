
#pragma once

#ifndef __TS3_GPUAPI_COMMON_GRAPHICS_IMMUTABLE_STATE_H__
#define __TS3_GPUAPI_COMMON_GRAPHICS_IMMUTABLE_STATE_H__

#include "../commonGraphicsConfig.h"
#include "../graphicsPipelineImmutableState.h"

namespace ts3::gpuapi
{

	struct BlendStateCommonProperties
	{
		Bitmask<ERTAttachmentFlags> activeAttachmentsMask = 0;
		Bitmask<EBlendWriteMaskFlags> attachment0WriteMask = 0;
		Bitmask<EBlendConfigFlags> flags = 0;
	};

	struct DepthStencilStateCommonProperties
	{
		bool depthTestActive;
		bool stencilTestActive;
		uint8 stencilReadMask;
		uint8 stencilWriteMask;
	};

	using RasterizerStateCommonProperties = RasterizerConfig;

	/// @brief
	class BlendImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		BlendStateCommonProperties const mCommonProperties;

	public:
		BlendImmutableState( const BlendImmutableState & ) = delete;
		BlendImmutableState & operator=( const BlendImmutableState & ) = delete;

		BlendImmutableState( GPUDevice & pGPUDevice, const BlendStateCommonProperties & pCommonProperties );
		virtual ~BlendImmutableState() = default;
	};

	/// @brief
	class DepthStencilImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		DepthStencilStateCommonProperties const mCommonProperties;

	public:
		DepthStencilImmutableState( const DepthStencilImmutableState & ) = delete;
		DepthStencilImmutableState & operator=( const DepthStencilImmutableState & ) = delete;

		DepthStencilImmutableState( GPUDevice & pGPUDevice, const DepthStencilStateCommonProperties & pCommonProperties );
		virtual ~DepthStencilImmutableState() = default;
	};

	/// @brief
	class RasterizerImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		RasterizerStateCommonProperties const mCommonProperties;

	public:
		RasterizerImmutableState( const RasterizerImmutableState & ) = delete;
		RasterizerImmutableState & operator=( const RasterizerImmutableState & ) = delete;

		RasterizerImmutableState( GPUDevice & pGPUDevice, const RasterizerStateCommonProperties & pCommonProperties );
		virtual ~RasterizerImmutableState() = default;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_GRAPHICS_IMMUTABLE_STATE_H__
