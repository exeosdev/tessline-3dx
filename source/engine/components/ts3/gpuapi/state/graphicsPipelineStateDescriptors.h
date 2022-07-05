
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__

#include "graphicsPipelineDescriptor.h"
#include "graphicsPipelineStateCommon.h"
#include "shaderBinding.h"

namespace ts3::gpuapi
{

	struct BlendStateDescriptorCreateInfo
	{
		BlendConfig blendConfig;
	};

	class BlendStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		std::unique_ptr<BlendConfig> const mBlendConfig;

	public:
		BlendStateDescriptor( GPUDevice & pGPUDevice,
		                      pipeline_descriptor_id_t pDescriptorID,
		                      const BlendConfig & pBlendConfig );

		virtual ~BlendStateDescriptor();


		TS3_FUNC_NO_DISCARD const BlendConfig * getSourceBlendConfig() const;

	private:
		std::unique_ptr<BlendConfig> _sourceBlendConfig;
	};

	struct DepthStencilStateDescriptorCreateInfo
	{
		DepthStencilConfig depthStencilConfig;
	};

	class DepthStencilStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		DepthStencilStateDescriptor( GPUDevice & pGPUDevice,
		                             pipeline_descriptor_id_t pDescriptorID,
		                             const DepthStencilConfig & pDepthStencilConfig );

		virtual ~DepthStencilStateDescriptor();

		TS3_FUNC_NO_DISCARD const DepthStencilConfig * getSourceDepthStencilConfig() const;

	private:
		std::unique_ptr<DepthStencilConfig> _sourceDepthStencilConfig;
	};

	struct GraphicsShaderBindingDescriptorCreateInfo
	{
		GraphicsShaderSet * shaderSet = nullptr;

		Bitmask<EShaderStageFlags> activeStagesMask = E_SHADER_STAGE_MASK_GRAPHICS_ALL;

		ArrayView<Shader *> shaderList;
	};

	class GraphicsShaderBindingDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		GraphicsShaderBindingDescriptor( GPUDevice & pGPUDevice,
		                                 pipeline_descriptor_id_t pDescriptorID,
		                                 const GraphicsShaderSet & pShaderSet );

		virtual ~GraphicsShaderBindingDescriptor();

		TS3_FUNC_NO_DISCARD const GraphicsShaderSet * getSourceGraphicsShaderSet() const;

	private:
		std::unique_ptr<GraphicsShaderSet> _sourceGraphicsShaderSet;
	};

	class RasterizerStateDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		RasterizerStateDescriptor( GPUDevice & pGPUDevice,
		                           pipeline_descriptor_id_t pDescriptorID,
		                           const RasterizerConfig & pRasterizerConfig );

		virtual ~RasterizerStateDescriptor();

		TS3_FUNC_NO_DISCARD const RasterizerConfig * getSourceRasterizerConfig() const;

	private:
		std::unique_ptr<RasterizerConfig> _sourceRasterizerConfig;
	};

	class RTAttachmentDescriptor : public GraphicsPipelineDescriptor
	{
	};

	class RTLayoutDescriptor : public GraphicsPipelineDescriptor
	{
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
