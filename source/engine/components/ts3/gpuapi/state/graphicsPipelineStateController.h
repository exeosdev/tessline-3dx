
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_CONTROLLER_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_CONTROLLER_H__

#include "graphicsPipelineConfigDefs.h"
#include <ts3/core/exceptionUtils.h>

namespace ts3::GpuAPI
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
		GraphicsPipelineStateController();
		virtual ~GraphicsPipelineStateController();

		/// @brief Binds the specified state object to the pipeline. Returns true if any change has been made.
		/// @return True if anything has been changed or false otherwise.
		/// @note Sub-classes should always call the base method first and check the result before doing the actual update.
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO );

		/// @brief Resets the current SO to the default one. Returns true if any change has been made.
		/// @return True if anything has been changed or false otherwise.
		/// @note Sub-classes should always call the base method first and check the result before doing the actual update.
		virtual bool resetGraphicsPipelineStateObject();

		/// @brief Binds the specified state object to the pipeline. Returns true if any change has been made.
		/// @return True if anything has been changed or false otherwise.
		/// @note Sub-classes should always call the base method first and check the result before doing the actual update.
		virtual bool setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO );

		/// @brief Resets the current SO to the default one. Returns true if any change has been made.
		/// @return True if anything has been changed or false otherwise.
		/// @note Sub-classes should always call the base method first and check the result before doing the actual update.
		virtual bool resetVertexStreamStateObject();

		/// @brief Returns true if the state update mask is not empty (some bits are set) or false otherwise.
		TS3_ATTR_NO_DISCARD bool isStateUpdateMaskSet() const
		{
			return !_stateUpdateMask.empty();
		}

		template <typename TpOutputSOType = GraphicsPipelineStateObject>
		inline const TpOutputSOType * getCurrentGraphicsPipelineSO() const noexcept
		{
			if( _currentGraphicsPipelineSO )
			{
				return reinterpret_cast<const GPUAPIObject *>( _currentGraphicsPipelineSO )->queryInterface<TpOutputSOType>();
			}
			return nullptr;
		}

		template <typename TpOutputSOType = GraphicsPipelineStateObject>
		inline const TpOutputSOType & getCurrentGraphicsPipelineSORef() const
		{
			if( const auto * stateObject = getCurrentGraphicsPipelineSO<TpOutputSOType>() )
			{
				return *stateObject;
			}
			throw 0;
		}

	protected:
		const GraphicsPipelineStateObject * _currentGraphicsPipelineSO = nullptr;

		Bitmask<graphics_state_update_mask_value_t> _stateUpdateMask = 0;
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_CONTROLLER_H__
