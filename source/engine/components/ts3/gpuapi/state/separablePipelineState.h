
#pragma once

#ifndef __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__
#define __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__

#include "graphicsPipelineStateController.h"
#include "pipelineStateObject.h"

namespace ts3::gpuapi
{

	enum EGraphicsStateUpdateSeparableFlags : graphics_state_update_mask_value_t
	{
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT = 0x0100,
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT = 0x0200,
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT = 0x0400,
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT = 0x0800,
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT = 0x1000,

		E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_STATES_ALL = 0x1F00,
		
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_VERTEX_BIT = 0x010000,
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_HULL_BIT = 0x020000,
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_DOMAIN_BIT = 0x040000,
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_GEOMETRY_BIT = 0x080000,
		E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_PIXEL_BIT = 0x100000,

		E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_SHADERS_ALL = 0x1F0000,

		E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_ALL =
			E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_STATES_ALL |
			E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_SHADERS_ALL,
		
		E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER =
			E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT |
			E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT,
	};

	struct SeparablePSOStateSet
	{
		const BlendImmutableState * blendState = nullptr;
		const DepthStencilImmutableState * depthStencilState = nullptr;
		const RasterizerImmutableState * rasterizerState = nullptr;
		const IAInputLayoutImmutableState * iaInputLayoutState = nullptr;
		const GraphicsShaderLinkageImmutableState * shaderLinkageState = nullptr;

		void reset()
		{
			blendState = nullptr;
			depthStencilState = nullptr;
			rasterizerState = nullptr;
			iaInputLayoutState = nullptr;
			shaderLinkageState = nullptr;
		}
	};

	struct SeparableShaderSet
	{
		Shader * vertexShader = nullptr;
		Shader * hullShader = nullptr;
		Shader * domainShader = nullptr;
		Shader * geometryShader = nullptr;
		Shader * pixelShader = nullptr;

		void reset()
		{
			vertexShader = nullptr;
			hullShader = nullptr;
			domainShader = nullptr;
			geometryShader = nullptr;
			pixelShader = nullptr;
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
	class TS3_GPUAPI_CLASS GraphicsPipelineStateObjectSeparable : public GraphicsPipelineStateObject
	{
	public:
		///
		SeparablePSOStateSet const mSeparableStates;

	public:
		GraphicsPipelineStateObjectSeparable(
				GPUDevice & pGPUDevice,
				RenderTargetLayout pRenderTargetLayout,
				ShaderInputSignature pShaderInputSignature,
				const SeparablePSOStateSet & pSeparableStates );

		virtual ~GraphicsPipelineStateObjectSeparable();
	};

	class TS3_GPUAPI_CLASS GraphicsPipelineStateObjectSeparableShader : public GraphicsPipelineStateObjectSeparable
	{
	public:
		///
		const GraphicsShaderSet & mSeparableShaders;

	public:
		GraphicsPipelineStateObjectSeparableShader(
				GPUDevice & pGPUDevice,
				RenderTargetLayout pRenderTargetLayout,
				ShaderInputSignature pShaderInputSignature,
				const SeparablePSOStateSet & pSeparableStates,
				const GraphicsShaderSet & pSeparableShaders );

		virtual ~GraphicsPipelineStateObjectSeparableShader();
	};

	/// @brief
	class TS3_GPUAPI_CLASS GraphicsPipelineStateControllerSeparable : public GraphicsPipelineStateController
	{
	public:
		GraphicsPipelineStateControllerSeparable();
		virtual ~GraphicsPipelineStateControllerSeparable();

		TS3_ATTR_NO_DISCARD const SeparablePSOStateSet & getCurrentSeparableStates() const noexcept;

		/// @see GraphicsPipelineStateController::setGraphicsPipelineStateObject
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO ) override;

		/// @see GraphicsPipelineStateController::resetGraphicsPipelineStateObject
		virtual bool resetGraphicsPipelineStateObject() override;

	private:
		Bitmask<uint64> setSeparablePSOStates( const GraphicsPipelineStateObjectSeparable & pGraphicsPSOSeparable );

	private:
		SeparablePSOStateSet _currentSeparableStates;
	};

	/// @brief
	class TS3_GPUAPI_CLASS GraphicsPipelineStateControllerSeparableShader : public GraphicsPipelineStateControllerSeparable
	{
	public:
		GraphicsPipelineStateControllerSeparableShader();
		virtual ~GraphicsPipelineStateControllerSeparableShader();

		TS3_ATTR_NO_DISCARD const SeparableShaderSet & getCurrentSeparableShaders() const noexcept;

		/// @see GraphicsPipelineStateController::setGraphicsPipelineStateObject
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO ) override;

		/// @see GraphicsPipelineStateController::resetGraphicsPipelineStateObject
		virtual bool resetGraphicsPipelineStateObject() override;

	private:
		Bitmask<uint64> setSeparableShaders( const GraphicsPipelineStateObjectSeparableShader & pGraphicsPSOSeparableShader );

	private:
		SeparableShaderSet _currentSeparableShaders;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__
