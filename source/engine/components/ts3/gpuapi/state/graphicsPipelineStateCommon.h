
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_CONFIG_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_CONFIG_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	enum EBlendFlags : uint32
	{
		E_BLEND_FLAG_ENABLE_ALPHA_TO_COVERAGE_BIT = 0x01,
		E_BLEND_FLAG_ENABLE_MRT_INDEPENDENT_BLEND_CONFIG_BIT = 0x02
	};

	struct ColorAttachmentBlendConfig
	{
		EActiveState blendActive;
		math::RGBAColorU8 constantFactor;
		EBlendFactor factorSrcColor;
		EBlendFactor factorSrcAlpha;
		EBlendFactor factorDstColor;
		EBlendFactor factorDstAlpha;
		EBlendOp opColor;
		EBlendOp opAlpha;
		EBlendRenderTargetWriteMask renderTargetWriteMask;
	};

	struct BlendConfig
	{
		ColorAttachmentBlendConfig colorAttachmentConfigs[E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		Bitmask<EBlendFlags> flags = 0;
	};

	struct DepthStencilConfig
	{
		struct DepthTestSettings
		{
			ECompFunc depthCompFunc;
			EDepthWriteMask depthWriteMask;
		};

		struct StencilFaceOp
		{
			ECompFunc compFunc;
			EStencilOp opFail;
			EStencilOp opPassDepthFail;
			EStencilOp opPassDepthPass;
		};

		struct StencilTestSettings
		{
			StencilFaceOp frontFace;
			StencilFaceOp backFace;
			uint8 refValue;
			uint8 readMask;
			uint8 writeMask;
		};

		EActiveState depthTestActive;
		DepthTestSettings depthTestSettings;
		EActiveState stencilTestActive;
		StencilTestSettings stencilTestSettings;
	};

	struct RasterizerConfig
	{
		ECullMode cullMode;
		EPrimitiveFillMode primitiveFillMode;
		ETriangleVerticesOrder triangleFrontFaceOrder;
		EActiveState scissorTestActive;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_CONFIG_H__
