
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_STATE_OBJECT_H__
#define __TS3_GPUAPI_PIPELINE_STATE_OBJECT_H__

#include "gpuStateObject.h"
#include "pipelineStateDesc.h"
#include "renderTargetCommon.h"
#include "shaderBinding.h"
#include "shaderInputSignature.h"

namespace ts3
{
namespace gpuapi
{

	struct ComputePipelineStateObjectCreateInfo
	{
	};

	struct GraphicsPipelineStateObjectCreateInfo
	{
		BlendConfigDesc blendDesc = cvPipelineDescBlendConfigDisabled;
		DepthStencilConfigDesc depthStencilDesc = cvPipelineDescDepthStencilConfigDisabled;
		RasterizerConfigDesc rasterizerDesc = cvPipelineDescRasterizerConfigDefault;
		VertexInputFormatDesc vertexInputFormatDesc = cvPipelineDescVertexInputFormatEmpty;
		RenderTargetLayoutDesc renderTargetLayoutDesc = cvRenderTargetLayoutDescDefaultBGRA8;
		RenderTargetLayout * renderTargetLayout = nullptr;
		GraphicsShaderBindingDesc * shaderBindingDesc = nullptr;
		ShaderInputSignatureDesc * shaderInputSignatureDesc = nullptr;
	};

	/// @brief A monolithic state object, containing subset of a certain GPU pipeline state.
	/// PipelineStateObject serves as a base class for all concrete PSO subtypes. There are two main types of PSOs:
	/// - Compute PSO for the async compute pipeline. Required for executing Dispatch*() calls.
	/// - Graphics PSO for the graphics/rendering pipeline. Required for executing all kind of Draw*() calls.
	/// Both have their dedicated classes: ComputePipelineStateObject and GraphicsPipelineStateObject, respectively.
	class TS3_GPUAPI_CLASS PipelineStateObject : public GPUStateObject
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

		GraphicsPipelineStateObject( GPUDevice & pGPUDevice,
		                             RenderTargetLayout pRenderTargetLayout,
		                             ShaderInputSignature pShaderInputSignature );

		virtual ~GraphicsPipelineStateObject();

	protected:
		// A struct for storing common state for the PSO.
		// Created by subclasses via createCommonPSOState().
		struct CommonPSOState
		{
			RenderTargetLayout renderTargetLayout;
			GraphicsShaderBinding shaderBinding;
			ShaderInputSignature shaderInputSignature;
		};

		// Validates common state desc and produces the actual state objects expected by the PSO:
		// - for GraphicsShaderBindingDesc, it validates the config and produces ready GraphicsShaderBinding object
		// - for ShaderInputSignatureDesc, it validates the config and build the actual ShaderInputSignature object
		static bool createCommonPSOState( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo, CommonPSOState & pOutputState );
	};

	/// @brief A more specific type of graphics PSO for drivers without monolithic PSO support.
	/// This class has been introduced for drivers which do not have the concept of monolithic PSOs, including:
	/// > DirectX 11 (with its ID3D11BlendState, ID3D11DepthStencilState, etc.)
	/// > OpenGL Core/ES (with no concept of state objects at all)
	/// For those drivers, PSOs contain an explicit "state descriptor" for each: blend, depth/stencil, rasterizer and
	/// vertex input state. Those descriptors are, of course, driver-specific (for DX11 they will have ID3D11XXXState
	/// interfaces, for OpenGL - bunch of translated state stored as a group of GL constants), but their IDs are stored
	/// at this common level. This, combined with a neat GraphicsPipelineStateDescriptorCache class, enable writing
	/// single implementation for caching and general state handling which can be used by the mentioned drivers.
	/// Additionally, separable PSOs also contain an explicit per-stage shader binding (which is part of the combined
	/// state in monolithic PSOs). This is another thing we can handle here instead of doing it per-driver.
	class TS3_GPUAPI_CLASS SeparableGraphicsPipelineStateObject : public GraphicsPipelineStateObject
	{
	public:
		GraphicsShaderBinding const mShaderBinding;
		GraphicsPipelineStateDescriptorSet const mSeparableDescriptorSet;

		SeparableGraphicsPipelineStateObject( GPUDevice & pGPUDevice,
		                                      RenderTargetLayout pRenderTargetLayout,
		                                      GraphicsShaderBinding pShaderBinding,
		                                      ShaderInputSignature pShaderInputSignature,
		                                      const GraphicsPipelineStateDescriptorSet & pSeparableDescriptorSet );

		virtual ~SeparableGraphicsPipelineStateObject();
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_PIPELINE_STATE_OBJECT_H__
