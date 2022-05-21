
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__
#define __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__

#include "pipelineStateDesc.h"
#include <ts3/core/exceptionUtils.h>

namespace ts3::gpuapi
{

	using graphics_state_update_mask_value_t = uint64;

	/// @brief Defines bit flags describing what kind of changes are pending for
	enum EGraphicsStateUpdateCommonFlags : graphics_state_update_mask_value_t
	{
		E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT = 0x1,

		E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT = 0x2,

		E_GRAPHICS_STATE_UPDATE_COMMON_SO_ALL_BITS_MASK = 0x3
	};

	/// @brief
	class TS3_GPUAPI_CLASS GraphicsPipelineStateController
	{
	public:
		using StateDescriptorID = pipeline_state_descriptor_id_t;

		GraphicsPipelineStateController();
		virtual ~GraphicsPipelineStateController();

		/// @brief
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject * pGraphicsPipelineSO );

		/// @brief
		virtual bool setVertexStreamStateObject( const VertexStreamStateObject * pVertexStreamSO );

		/// @brief
		virtual void applyPendingPipelineStateChange();

	protected:
		struct StateObjects
		{
			const GraphicsPipelineStateObject * graphicsPipelineSO = nullptr;
			const VertexStreamStateObject * vertexStreamSO = nullptr;

			void reset()
			{
				graphicsPipelineSO = nullptr;
				vertexStreamSO = nullptr;
			}
		};

		StateObjects _currentStateObjects;

		StateObjects _pendingStateObjects;

		Bitmask<graphics_state_update_mask_value_t> _stateUpdateMask = 0;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PIPELINE_STATE_CONTROLLER_H__
