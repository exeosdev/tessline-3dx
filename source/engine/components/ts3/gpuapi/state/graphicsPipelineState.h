
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__

#include "graphicsPipelineConfigDefs.h"
#include "graphicsShaderLinkage.h"
#include "inputAssemblerCommon.h"

namespace ts3::GpuAPI
{

	enum EBlendStateFlags : uint32
	{
		E_BLEND_STATE_FLAG_ACTIVE_BIT = 0x01,
		E_BLEND_STATE_FLAG_ALPHA_TO_COVERAGE_BIT = 0x02,
		E_BLEND_STATE_FLAG_MRT_INDEPENDENT_BLEND_CONFIG_BIT = 0x04,
		E_BLEND_STATE_FLAG_WRITE_MASK_CHANNEL_RED = E_BLEND_WRITE_MASK_CHANNEL_RED,
		E_BLEND_STATE_FLAG_WRITE_MASK_CHANNEL_GREEN = E_BLEND_WRITE_MASK_CHANNEL_GREEN,
		E_BLEND_STATE_FLAG_WRITE_MASK_CHANNEL_BLUE = E_BLEND_WRITE_MASK_CHANNEL_BLUE,
		E_BLEND_STATE_FLAG_WRITE_MASK_CHANNEL_ALPHA = E_BLEND_WRITE_MASK_CHANNEL_ALPHA,
		E_BLEND_STATE_MASK_ALL = 0xF7
	};

	enum EDepthStencilStateFlags : uint32
	{
		E_DEPTH_STENCIL_STATE_FLAG_DEPTH_TEST_ACTIVE_BIT = 0x01,
		E_DEPTH_STENCIL_STATE_FLAG_DEPTH_WRITE_MASK_ALL_BIT = 0x02,
		E_DEPTH_STENCIL_STATE_FLAG_DEPTH_WRITE_MASK_ZERO_BIT = 0x04,
		E_DEPTH_STENCIL_STATE_FLAG_STENCIL_TEST_ACTIVE_BIT = 0x08,
		E_DEPTH_STENCIL_STATE_MASK_ALL = 0x0F
	};

	enum ERasterizerStateFlags : uint32
	{
		E_RASTERIZER_STATE_FLAG_CULL_ACTIVE_BIT = 0x01,
		E_RASTERIZER_STATE_FLAG_CULL_MODE_BACK_BIT = 0x02 | E_RASTERIZER_STATE_FLAG_CULL_ACTIVE_BIT,
		E_RASTERIZER_STATE_FLAG_CULL_MODE_FRONT_BIT = 0x04 | E_RASTERIZER_STATE_FLAG_CULL_ACTIVE_BIT,
		E_RASTERIZER_STATE_FLAG_SCISSOR_TEST_ACTIVE_BIT = 0x08,
		E_RASTERIZER_STATE_FLAG_PRIMITIVE_FILL_MODE_SOLID_BIT = 0x10,
		E_RASTERIZER_STATE_FLAG_PRIMITIVE_FILL_MODE_WIREFRAME_BIT = 0x20,
		E_RASTERIZER_STATE_FLAG_VERTICES_ORDER_CLOCKWISE_BIT = 0x40,
		E_RASTERIZER_STATE_FLAG_VERTICES_ORDER_COUNTER_CLOCKWISE_BIT = 0x80,
		E_RASTERIZER_STATE_MASK_ALL = 0xF7
	};

	class BlendState
	{
	public:
		Bitmask<EBlendStateFlags> const mBlendStateFlags;
	};

	class DepthStencilState
	{
	public:
		Bitmask<EDepthStencilStateFlags> const mDepthStencilStateFlags;
	};

	class RasterizerState
	{
	public:
		Bitmask<ERasterizerStateFlags> const mRasterizerStateFlags;
	};

	// State Management Utility API
	namespace StateMgmt
	{

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EBlendStateFlags> translateBlendConfigToStateFlags(
				const BlendConfig & pBlendConfig );

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EDepthStencilStateFlags> translateDepthStencilConfigToStateFlags(
				const DepthStencilConfig & pDepthStencilConfig );

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD Bitmask<ERasterizerStateFlags> translateBlendConfigToStateFlags(
				const RasterizerConfig & pRasterizerConfig );

	}

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
