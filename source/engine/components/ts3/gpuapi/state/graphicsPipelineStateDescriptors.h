
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__

#include "graphicsPipelineDescriptor.h"
#include "graphicsPipelineStateCommon.h"
#include "shaderBinding.h"
#include <ts3/stdext/hash.h>

namespace ts3::gpuapi
{

	struct BlendStateDescriptorCreateInfo
	{
		BlendConfig blendConfig;
	};

	class BlendStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		BlendConfig const mBlendConfig;
	};

	struct DepthStencilStateDescriptorCreateInfo
	{
		DepthStencilConfig depthStencilConfig;
	};

	class DepthStencilStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		DepthStencilConfig const mDepthStencilConfig;
	};

	struct GraphicsShaderBindingDescriptorCreateInfo
	{
		Bitmask<EShaderStageFlags> activeStagesMask = E_SHADER_STAGE_MASK_GRAPHICS_ALL;

		ArrayView<Shader *> shaderList;

		GraphicsShaderSet * shaderSet = nullptr;
	};

	class GraphicsShaderBindingDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		GraphicsShaderSet const mShaderSet;
	};

	class RasterizerStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		RasterizerConfig const mRasterizerConfig;
	};

	class RTAttachmentDescriptor : public GraphicsPipelineDescriptor
	{
	};

	class RTLayoutDescriptor : public GraphicsPipelineDescriptor
	{
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
