
#pragma once

#ifndef __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__
#define __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__

#include "pipelineStateController.h"
#include "pipelineStateObject.h"

namespace ts3::gpuapi
{

	enum EGraphicsStateUpdateSeparableFlags : graphics_state_update_mask_value_t
	{
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_ANY_BIT = 0x0010,

		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT = 0x0020,
		
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_BLEND_BIT = 0x0100 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_ANY_BIT,
		
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_DEPTH_STENCIL_BIT = 0x0200 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_ANY_BIT,
		
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_RASTERIZER_BIT = 0x0400 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_ANY_BIT,
		
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT = 0x0800 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_ANY_BIT,
		
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_VERTEX_STAGE_BIT = 0x010000 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT,
				
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_TESS_CONTROL_STAGE_BIT = 0x020000 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT,
				
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_TESS_EVALUATION_STAGE_BIT = 0x040000 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT,
				
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_GEOMETRY_STAGE_BIT = 0x080000 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT,
				
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_AMPLIFICATION_STAGE_BIT = 0x100000 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT,
				
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_MESH_STAGE_BIT = 0x200000 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT,
				
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_PIXEL_STAGE_BIT = 0x400000 | E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT,
				
		E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ALL_BITS_MASK = 0x7F0000
	};

	/// @brief Contains a set of descriptor IDs which together describe the state of a separable graphics pipeline
	/// (without monolithic PSOs).
	struct SeparableGraphicsStateDescriptorSet
	{
		/// ID of the blend state descriptor.
		pipeline_state_descriptor_id_t blendDescriptorID = CX_PIPELINE_STATE_DESCRIPTOR_ID_INVALID;

		/// ID of the depth/stencil state descriptor.
		pipeline_state_descriptor_id_t depthStencilDescriptorID = CX_PIPELINE_STATE_DESCRIPTOR_ID_INVALID;

		/// ID of the rasterizer state descriptor.
		pipeline_state_descriptor_id_t rasterizerDescriptorID = CX_PIPELINE_STATE_DESCRIPTOR_ID_INVALID;

		/// ID of the vertex input state descriptor.
		pipeline_state_descriptor_id_t vertexInputFormatDescriptorID = CX_PIPELINE_STATE_DESCRIPTOR_ID_INVALID;

		void reset()
		{
			blendDescriptorID = CX_PIPELINE_STATE_DESCRIPTOR_ID_INVALID;
			depthStencilDescriptorID = CX_PIPELINE_STATE_DESCRIPTOR_ID_INVALID;
			rasterizerDescriptorID = CX_PIPELINE_STATE_DESCRIPTOR_ID_INVALID;
			vertexInputFormatDescriptorID = CX_PIPELINE_STATE_DESCRIPTOR_ID_INVALID;
		}
	};

	/// @brief Contains a set of graphics shaders which are bound to a separable graphics pipeline.
	struct SeparableGraphicsShaderBinding
	{
		/// A pointer to a shader object used for the vertex stage of the "classic" (V1) shader pipeline.
		Shader * vertexShader = nullptr;

		/// A pointer to a shader object used for the tesselation control stage of the "classic" (V1) shader pipeline.
		Shader * tessControlShader = nullptr;

		/// A pointer to a shader object used for the tesselation evaluation stage of the "classic" (V1) shader pipeline.
		Shader * tessEvaluationShader = nullptr;

		/// A pointer to a shader object used for the geometry stage of the "classic" (V1) shader pipeline.
		Shader * geometryShader = nullptr;

		/// A pointer to a shader object used for the amplification stage of the new (V2) shader pipeline.
		Shader * amplificationShader = nullptr;

		/// A pointer to a shader object used for the mesh stage of the new (V2) shader pipeline.
		Shader * meshShader = nullptr;

		/// A pointer to a shader object used for the pixel stage.
		Shader * pixelShader = nullptr;

		void reset()
		{
			vertexShader = nullptr;
			tessControlShader = nullptr;
			tessEvaluationShader = nullptr;
			geometryShader = nullptr;
			amplificationShader = nullptr;
			meshShader = nullptr;
			pixelShader = nullptr;
		}
	};

	/// @brief A more specific type of GraphicsPSO for drivers without monolithic state object support.
	///
	/// This class has been introduced for drivers which do not have the concept of monolithic PSOs, including:
	/// (1) DirectX 11 (with its ID3D11BlendState, ID3D11DepthStencilState, etc),
	/// (2) OpenGL Core/ES (with no concept of state objects at all).
	/// For those drivers, PSOs contain an explicit "state descriptor" for each: blend, depth/stencil, rasterizer and
	/// vertex input state. Those descriptors are, of course, driver-specific (for DX11 they will have ID3D11XXXState
	/// interfaces, for OpenGL - bunch of translated state stored as a group of GL constants), but their IDs are stored
	/// at this common level. This, combined with a neat GraphicsPipelineStateDescriptorCache class, enables writing
	/// single implementation for caching and general state handling which can be used by the mentioned drivers.
	/// Additionally, separable PSOs also contain an explicit per-stage shader binding (which is part of the combined
	/// state in monolithic PSOs). This is another thing we can handle here instead of doing it per-driver.
	class TS3_GPUAPI_CLASS SeparableGraphicsPipelineStateObject : public GraphicsPipelineStateObject
	{
	public:
		GraphicsShaderBinding const mShaderBinding;

		SeparableGraphicsStateDescriptorSet const mSeparableDescriptorSet;

		SeparableGraphicsPipelineStateObject( GPUDevice & pGPUDevice,
		                                      RenderTargetLayout pRenderTargetLayout,
		                                      GraphicsShaderBinding pShaderBinding,
		                                      ShaderInputSignature pShaderInputSignature,
		                                      const SeparableGraphicsStateDescriptorSet & pStateDescriptors );

		virtual ~SeparableGraphicsPipelineStateObject();
	};

	/// @brief
	class TS3_GPUAPI_CLASS SeparableGraphicsPipelineStateController : public GraphicsPipelineStateController
	{
	public:
		SeparableGraphicsPipelineStateController();
		virtual ~SeparableGraphicsPipelineStateController();

		/// @brief
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject * pGraphicsPipelineSO ) override;

		/// @brief
		virtual bool setVertexStreamStateObject( const VertexStreamStateObject * pVertexStreamSO ) override;

		/// @brief
		virtual void applyPendingPipelineStateChange() override;

	private:
		Bitmask<uint64> setGraphicsPSODescriptors( const SeparableGraphicsPipelineStateObject & pSeparableGSPO );

		Bitmask<uint64> setGraphicsPSOShaders( const SeparableGraphicsPipelineStateObject & pSeparableGSPO );

	protected:
		struct PipelineConfig
		{
			SeparableGraphicsShaderBinding shaderBinding;
			SeparableGraphicsStateDescriptorSet stateDescriptors;

			void reset()
			{
				shaderBinding.reset();
				stateDescriptors.reset();
			}
		};

		PipelineConfig _currentPipelineConfig;
		PipelineConfig _pendingPipelineConfig;
	};
} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SEPARABLE_PIPELINE_STATE_H__
