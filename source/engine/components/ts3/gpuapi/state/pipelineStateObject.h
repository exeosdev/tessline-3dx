
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_STATE_OBJECT_H__
#define __TS3_GPUAPI_PIPELINE_STATE_OBJECT_H__

#include "graphicsPipelineImmutableState.h"
#include "inputAssemblerCommon.h"
#include "graphicsShaderState.h"
#include "renderTargetCommon.h"
#include "shaderInputSignature.h"

namespace ts3::gpuapi
{

	struct ComputePipelineStateObjectCreateInfo
	{
	};

	struct GraphicsPipelineStateObjectCreateInfo
	{
		BlendConfig blendConfig;
		mutable BlendImmutableStateHandle blendState;

		DepthStencilConfig depthStencilConfig;
		mutable DepthStencilImmutableStateHandle depthStencilState;

		RasterizerConfig rasterizerConfig;
		mutable RasterizerImmutableStateHandle rasterizerState;

		GraphicsShaderSet shaderSet;
		mutable GraphicsShaderLinkageImmutableStateHandle shaderLinkageState;

		IAInputLayoutDefinition inputLayoutDefinition;
		mutable IAInputLayoutImmutableStateHandle inputLayoutState;

		RenderTargetLayout * renderTargetLayout = nullptr;

		ShaderInputSignatureDesc shaderInputSignatureDesc;
		mutable ShaderInputSignature shaderInputSignature;
	};

	/// @brief A monolithic state object, containing subset of a certain GPU pipeline state.
	/// PipelineStateObject serves as a base class for all concrete PSO subtypes. There are two main types of PSOs:
	/// - Compute PSO for the async compute pipeline. Required for executing Dispatch*() calls.
	/// - Graphics PSO for the graphics/rendering pipeline. Required for executing all kind of Draw*() calls.
	/// Both have their dedicated classes: ComputePipelineStateObject and GraphicsPipelineStateObject, respectively.
	class TS3_GPUAPI_CLASS PipelineStateObject : public GPUDeviceChildObject
	{
	public:
		PipelineStateObject( GPUDevice & pGPUDevice );
		virtual ~PipelineStateObject();
	};

	/// @brief PSO for the compute pipeline.
	class TS3_GPUAPI_CLASS ComputePipelineStateObject : public PipelineStateObject
	{
	public:
		ComputePipelineStateObject( GPUDevice & pGPUDevice );
		virtual ~ComputePipelineStateObject();
	};

	/// @brief PSO for the graphics pipeline.
	/// Contains a set of state required by the rendering pipeline, stored in either generic or driver-specific format.
	/// Graphics PSO contains the following state (see also GraphicsPipelineStateObjectCreateInfo):
	/// 1) Generic state (stored in the common format):
	/// > render target configuration
	/// > shader binding state
	/// > shader input signature
	/// 2) Driver-specific state (created at the driver level, translated to its native form):
	/// > blend state
	/// > depth/stencil state
	/// > rasterizer state
	/// > vertex input format
	class TS3_GPUAPI_CLASS GraphicsPipelineStateObject : public PipelineStateObject
	{
	public:
		RenderTargetLayout const mRenderTargetLayout;
		ShaderInputSignature const mShaderInputSignature;

		GraphicsPipelineStateObject(
			GPUDevice & pGPUDevice,
			RenderTargetLayout pRenderTargetLayout,
			ShaderInputSignature pShaderInputSignature );

		virtual ~GraphicsPipelineStateObject();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PIPELINE_STATE_OBJECT_H__
