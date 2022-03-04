
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__
#define __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__

#include "pipelineStateDesc.h"

namespace ts3
{
namespace gpuapi
{

	using graphics_state_update_mask_value_t = uint64;

	enum : graphics_state_update_mask_value_t
	{
		E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT    = 1u << 0x10,
		E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT        = 1u << 0x11,
		E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_BLEND_BIT               = 1u << 0,
		E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_DEPTH_STENCIL_BIT       = 1u << 1,
		E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_RASTERIZER_BIT          = 1u << 2,
		E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT = 1u << 3,
		E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT               = 1u << 4,
		E_GRAPHICS_STATE_UPDATE_SHADER_VERTEX_STAGE_BIT            = ( 1u << 5 ) | E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SHADER_TESS_CONTROL_STAGE_BIT      = ( 1u << 6 ) | E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SHADER_TESS_EVALUATION_STAGE_BIT   = ( 1u << 7 ) | E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SHADER_GEOMETRY_STAGE_BIT          = ( 1u << 8 ) | E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SHADER_PIXEL_STAGE_BIT             = ( 1u << 9 ) | E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT,
		E_GRAPHICS_STATE_UPDATE_SHADER_ALL_MASK                    = 0x3F0
	};

	/// @brief
	class TS3_GPUAPI_CLASS GraphicsPipelineStateController
	{
	public:
		struct CommonConfig
		{
			const GraphicsPipelineStateObject * soGraphicsPipeline = nullptr;
			const VertexStreamStateObject * soVertexStream = nullptr;
		};

	public:
		GraphicsPipelineStateController();
		virtual ~GraphicsPipelineStateController();

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO );

		virtual bool setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO );

		virtual void resetInternalState();

		const CommonConfig & getCommonConfig() const
		{
			return _csCommonConfig;
		}

	protected:
		virtual bool updatePipelineState();

	protected:
		CommonConfig _csCommonConfig;
		Bitmask<graphics_state_update_mask_value_t> _stateUpdateMask = 0;
	};

	class TS3_GPUAPI_CLASS SeparableGraphicsPipelineStateController : public GraphicsPipelineStateController
	{
	public:
		struct SeparableShaderBinding
		{
			Shader * vertexShader = nullptr;
			Shader * tessControlShader = nullptr;
			Shader * tessEvaluationShader = nullptr;
			Shader * geometryShader = nullptr;
			Shader * pixelShader = nullptr;
		};

		struct SeparableStateDescriptorSet
		{
			pipeline_state_descriptor_id_t blendDescriptorID = cxInvalidPipelineStateDescriptorID;
			pipeline_state_descriptor_id_t depthStencilDescriptorID = cxInvalidPipelineStateDescriptorID;
			pipeline_state_descriptor_id_t rasterizerDescriptorID = cxInvalidPipelineStateDescriptorID;
			pipeline_state_descriptor_id_t vertexInputFormatDescriptorID = cxInvalidPipelineStateDescriptorID;
		};

	public:
		SeparableGraphicsPipelineStateController();
		virtual ~SeparableGraphicsPipelineStateController();

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) override;

		virtual void resetInternalState() override;

		const SeparableShaderBinding & getSeparableShaderBinding() const
		{
			return _csSeparableShaderBinding;
		}

		const SeparableStateDescriptorSet & getSeparableStateDescriptorSet() const
		{
			return _csSeparableStateDescriptorSet;
		}

	protected:
		SeparableShaderBinding _csSeparableShaderBinding;
		SeparableStateDescriptorSet _csSeparableStateDescriptorSet;
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__
