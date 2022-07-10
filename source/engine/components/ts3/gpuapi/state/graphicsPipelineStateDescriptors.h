
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__

#include "graphicsPipelineDescriptor.h"
#include "graphicsPipelineStateCommon.h"
#include "graphicsShaderBinding.h"

namespace ts3::gpuapi
{

	enum EBlendStateFlags : uint32
	{
		E_BLEND_STATE_FLAG_ACTIVE_BIT = 0x01,
		E_BLEND_STATE_FLAG_ALPHA_TO_COVERAGE_BIT = 0x02,
		E_BLEND_STATE_FLAG_MRT_INDEPENDENT_BLEND_CONFIG_BIT = 0x04,
		E_BLEND_STATE_MASK_ALL = 0x07
	};

	enum EDepthStencilStateFlags : uint32
	{
		E_DEPTH_STENCIL_STATE_FLAG_DEPTH_TEST_ACTIVE_BIT = 0x01,
		E_DEPTH_STENCIL_STATE_FLAG_STENCIL_TEST_ACTIVE_BIT = 0x02
	};

	enum ERasterizerStateFlags : uint32
	{
		E_RASTERIZER_STATE_FLAG_CULL_ACTIVE_BIT = 0x01,
		E_RASTERIZER_STATE_FLAG_CULL_MODE_BACK_BIT = 0x02 | E_RASTERIZER_STATE_FLAG_CULL_ACTIVE_BIT,
		E_RASTERIZER_STATE_FLAG_CULL_MODE_FRONT_BIT = 0x04 | E_RASTERIZER_STATE_FLAG_CULL_ACTIVE_BIT,
		E_RASTERIZER_STATE_FLAG_SCISSOR_TEST_ACTIVE_BIT = 0x10,
		E_RASTERIZER_STATE_FLAG_PRIMITIVE_FILL_MODE_SOLID_BIT = 0x100,
		E_RASTERIZER_STATE_FLAG_PRIMITIVE_FILL_MODE_WIREFRAME_BIT = 0x200,
		E_RASTERIZER_STATE_FLAG_VERTICES_ORDER_CLOCKWISE_BIT = 0x400,
		E_RASTERIZER_STATE_FLAG_VERTICES_ORDER_COUNTER_CLOCKWISE_BIT = 0x800,
	};

	struct BlendStateDescriptorCreateInfo
	{
		BlendConfig blendConfig;
	};

	struct DepthStencilStateDescriptorCreateInfo
	{
		DepthStencilConfig depthStencilConfig;
	};

	struct RasterizerStateDescriptorCreateInfo
	{
		RasterizerConfig rasterizerConfig;
	};

	class BlendStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		Bitmask<EBlendStateFlags> const mBlendStateFlags;

	public:
		BlendStateDescriptor( GPUDevice & pGPUDevice,
		                      pipeline_descriptor_id_t pDescriptorID,
		                      Bitmask<EBlendStateFlags> pBlendStateFlags );

		virtual ~BlendStateDescriptor();
	};

	class DepthStencilStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		Bitmask<EDepthStencilStateFlags> const mDepthStencilStateFlags;

	public:
		DepthStencilStateDescriptor( GPUDevice & pGPUDevice,
		                             pipeline_descriptor_id_t pDescriptorID,
		                             Bitmask<EDepthStencilStateFlags> pDepthStencilStateFlags );

		virtual ~DepthStencilStateDescriptor();
	};

	class RasterizerStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		Bitmask<ERasterizerStateFlags> const mRasterizerStateFlags;

	public:
		RasterizerStateDescriptor( GPUDevice & pGPUDevice,
		                           pipeline_descriptor_id_t pDescriptorID,
		                           Bitmask<ERasterizerStateFlags> pRasterizerStateFlags );

		virtual ~RasterizerStateDescriptor();
	};

	// State Management Utils
	namespace smu
	{

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EBlendStateFlags> translateBlendConfigToStateFlags( const BlendConfig & pBlendConfig );

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EDepthStencilStateFlags> translateDepthStencilConfigToStateFlags(
				const DepthStencilConfig & pDepthStencilConfig );

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD Bitmask<ERasterizerStateFlags> translateBlendConfigToStateFlags(
				const RasterizerConfig & pRasterizerConfig );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
