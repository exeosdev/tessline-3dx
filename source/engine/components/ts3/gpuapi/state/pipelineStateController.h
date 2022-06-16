
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__
#define __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__

#include "pipelineStateDesc.h"
#include <ts3/core/exceptionUtils.h>

namespace ts3::gpuapi
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
		GraphicsPipelineStateController();
		virtual ~GraphicsPipelineStateController();

		bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject * pGraphicsPipelineSO );

		bool setVertexStreamStateObject( const VertexStreamStateObject * pVertexStreamSO );

		void resetInternalState();

		TS3_FUNC_NO_DISCARD const GraphicsPipelineStateObject * getGraphicsPipelineStateObject() const noexcept
		{
			return _graphicsPipelineStateObject;
		}

		TS3_FUNC_NO_DISCARD const VertexStreamStateObject * getVertexStreamStateObject() const noexcept
		{
			return _vertexStreamStateObject;
		}

		TS3_FUNC_NO_DISCARD const GraphicsPipelineStateObject & getGraphicsPipelineStateObjectRef() const
		{
			ts3ThrowIfNull( _graphicsPipelineStateObject, E_EXC_DEBUG_PLACEHOLDER );
			return *_graphicsPipelineStateObject;
		}

		TS3_FUNC_NO_DISCARD const VertexStreamStateObject & getVertexStreamStateObjectRef() const
		{
			ts3ThrowIfNull( _vertexStreamStateObject, E_EXC_DEBUG_PLACEHOLDER );
			return *_vertexStreamStateObject;
		}

	protected:
		virtual bool updatePipelineState();

		virtual bool setGraphicsPipelineStateObjectInternal( const GraphicsPipelineStateObject * pGraphicsPipelineSO );

		virtual bool setVertexStreamStateObjectInternal( const VertexStreamStateObject * pVertexStreamSO );

	protected:
		const GraphicsPipelineStateObject * _graphicsPipelineStateObject = nullptr;

		const VertexStreamStateObject * _vertexStreamStateObject = nullptr;

		Bitmask<graphics_state_update_mask_value_t> _stateUpdateMask = 0;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__
