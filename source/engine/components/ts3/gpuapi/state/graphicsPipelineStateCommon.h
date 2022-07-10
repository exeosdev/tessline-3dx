
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_CONFIG_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_CONFIG_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	enum EBlendConfigFlags : uint32
	{
		E_BLEND_CONFIG_FLAG_ENABLE_ALPHA_TO_COVERAGE_BIT = 0x01,
		E_BLEND_CONFIG_FLAG_ENABLE_MRT_INDEPENDENT_BLEND_CONFIG_BIT = 0x02
	};

	struct ColorAttachmentBlendConfig
	{
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
		EActiveState blendActive;
		ColorAttachmentBlendConfig colorAttachmentConfigs[E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		Bitmask<EBlendConfigFlags> flags;
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
