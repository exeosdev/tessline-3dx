
#pragma once

#ifndef __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__
#define __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__

#include "graphicsPipelineStateController.h"
#include "pipelineStateObject.h"
#include "separablePipelineImmutableState.h"

namespace ts3::gpuapi
{

	enum EGraphicsStateUpdateSeparableFlags : graphics_state_update_mask_value_t
	{
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_BLEND_BIT = 0x01,
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_DEPTH_STENCIL_BIT = 0x02,
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_RASTERIZER_BIT = 0x04,
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT = 0x08,
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_SHADER_LINKAGE_BIT = 0x10,

		E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATES_ALL = 0x1F,
		
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_VERTEX_STAGE_BIT = 0x0100 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_SHADER_LINKAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_HULL_STAGE_BIT = 0x0200 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_SHADER_LINKAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_DOMAIN_STAGE_BIT = 0x0400 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_SHADER_LINKAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_GEOMETRY_STAGE_BIT = 0x0800 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_SHADER_LINKAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_PIXEL_STAGE_BIT = 0x1000 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_SHADER_LINKAGE_BIT,

		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADERS_ALL = 0x1F00 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_STATE_SHADER_LINKAGE_BIT
	};

	struct SeparableGraphicsImmutableStateSet
	{
		BlendImmutableStateHandle blendState;
		DepthStencilImmutableStateHandle depthStencilState;
		RasterizerImmutableStateHandle rasterizerState;
		IAInputLayoutImmutableStateHandle iaInputLayoutState;
		SeparableGraphicsShaderImmutableStateHandle shaderLinkageState;

		void reset()
		{
			blendState = nullptr;
			depthStencilState = nullptr;
			rasterizerState = nullptr;
			iaInputLayoutState = nullptr;
			shaderLinkageState = nullptr;
		}
	};

	/// @brief A more specific type of GraphicsPSO for drivers without monolithic state object support.
	///
	/// This class has been introduced for drivers which do not have the concept of monolithic PSOs, including:
	/// (1) DirectX 11 (with its ID3D11BlendState, ID3D11DepthStencilState, etc),
	/// (2) OpenGL Core/ES (with no concept of state objects at all).
	/// For those drivers, PSOs contain an explicit "state descriptor" for each: blend, depth/stencil, rasterizer and
	/// vertex input state. Those pdesc are, of course, driver-specific (for DX11 they will have ID3D11XXXState
	/// interfaces, for OpenGL - bunch of translated state stored as a group of GL constants), but their IDs are stored
	/// at this common level. This, combined with a neat GraphicsPipelineStateDescriptorCache class, enables writing
	/// single implementation for caching and general state handling which can be used by the mentioned drivers.
	/// Additionally, separable PSOs also contain an explicit per-stage shader binding (which is part of the combined
	/// state in monolithic PSOs). This is another thing we can handle here instead of doing it per-driver.
	class TS3_GPUAPI_CLASS SeparableGraphicsPipelineStateObject : public GraphicsPipelineStateObject
	{
	public:
		///
		SeparableGraphicsImmutableStateSet const mSeparableStates;
		///
		const GraphicsShaderSet & mShaderSet;

		SeparableGraphicsPipelineStateObject( GPUDevice & pGPUDevice,
		                                      RenderTargetLayout pRenderTargetLayout,
		                                      ShaderInputSignature pShaderInputSignature,
		                                      const SeparableGraphicsImmutableStateSet & pPSOImmutableStates );

		virtual ~SeparableGraphicsPipelineStateObject();
	};

	/// @brief
	class TS3_GPUAPI_CLASS SeparableGraphicsPipelineStateController : public GraphicsPipelineStateController
	{
	public:
		SeparableGraphicsPipelineStateController();
		virtual ~SeparableGraphicsPipelineStateController();

		// Below methods are more concrete implementations of their base version defined in GraphicsPipelineStateController.
		// They provide some common logic for separate pipelines which allows extra validation and state checking to be
		// implemented only once for all separate pipelines-based APIs.

		/// @brief A more specific implementation of setGraphicsPipelineStateObject() for separable pipelines.
		/// @see GraphicsPipelineStateController::setGraphicsPipelineStateObject
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) override;

		/// @brief A more specific implementation of resetGraphicsPipelineStateObject() for separable pipelines.
		/// @see GraphicsPipelineStateController::resetGraphicsPipelineStateObject
		virtual bool resetGraphicsPipelineStateObject() override;

		TS3_ATTR_NO_DISCARD const SeparableGraphicsShaderBinding & getSeparableShaderBinding() const
		{
			return _currentSeparableShaderBinding;
		}

		TS3_ATTR_NO_DISCARD const SeparableGraphicsStateDescriptorSet & getSeparableStateDescriptorSet() const
		{
			return _currentSeparableStateDescriptors;
		}

	private:
		Bitmask<uint64> setGraphicsPSODescriptors( const SeparableGraphicsPipelineStateObject & pSeparableGSPO );

		Bitmask<uint64> setGraphicsPSOShaders( const SeparableGraphicsPipelineStateObject & pSeparableGSPO );

	protected:
		SeparableGraphicsShaderBinding _currentSeparableShaderBinding;
		SeparableGraphicsStateDescriptorSet _currentSeparableStateDescriptors;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__
