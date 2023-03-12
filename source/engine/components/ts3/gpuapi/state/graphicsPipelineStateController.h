
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_CONTROLLER_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_CONTROLLER_H__

#include "commonGPUStateDefs.h"
#include <ts3/core/exceptionUtils.h>

namespace ts3::gpuapi
{

	class PipelineImmutableStateFactory;

	using graphics_state_update_mask_value_t = uint64;

	/// @brief Defines bit flags describing what kind of changes are pending for
	enum EGraphicsStateUpdateCommonFlags : graphics_state_update_mask_value_t
	{
		E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT = 0x01,
		E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT = 0x02,
		E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT = 0x04,

		E_GRAPHICS_STATE_UPDATE_MASK_COMMON_ALL = 0x07
	};

	/// @brief
	class TS3_GPUAPI_CLASS GraphicsPipelineStateController
	{
	public:
		GraphicsPipelineStateController();
		virtual ~GraphicsPipelineStateController();

		virtual void applyPipelineStateChanges() = 0;

		/// @brief Binds the specified state object to the pipeline. Returns true if any change has been made.
		/// @return True if anything has been changed or false otherwise.
		/// @note Sub-classes should always call the base method first and check the result before doing the actual update.
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO );

		/// @brief Resets the current SO to the default one. Returns true if any change has been made.
		/// @return True if anything has been changed or false otherwise.
		/// @note Sub-classes should always call the base method first and check the result before doing the actual update.
		virtual bool resetGraphicsPipelineStateObject();

		virtual bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState );
		virtual bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState );
		virtual bool resetIAVertexStreamState();

		virtual bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState );
		virtual bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState );
		virtual bool resetRenderTargetBindingState();

		/// @brief Returns true if the state update mask is not empty (some bits are set) or false otherwise.
		TS3_ATTR_NO_DISCARD bool hasPendingState() const
		{
			return !_stateUpdateMask.empty();
		}

		template <typename TGraphicsPipelineStateObject = GraphicsPipelineStateObject>
		inline const TGraphicsPipelineStateObject * getCurrentGraphicsPSO() const noexcept
		{
			if( _currentCommonState.graphicsPSO )
			{
				const auto * baseInterface = reinterpret_cast<const GPUAPIObject *>( _currentCommonState.graphicsPSO );
				return baseInterface->queryInterface<TGraphicsPipelineStateObject>();
			}
			return nullptr;
		}

		template <typename TGraphicsPipelineStateObject = GraphicsPipelineStateObject>
		inline const TGraphicsPipelineStateObject & getCurrentGraphicsPSORef() const
		{
			if( const auto * stateObject = getCurrentGraphicsPSO<TGraphicsPipelineStateObject>() )
			{
				return *stateObject;
			}
			ts3Throw( 0 );
		}

		template <typename TIAVertexStreamState = IAVertexStreamImmutableState>
		inline const TIAVertexStreamState * getCurrentIAVertexStreamState() const noexcept
		{
			if( _currentCommonState.iaVertexStreamState )
			{
				const auto * baseInterface = reinterpret_cast<const GPUAPIObject *>( _currentCommonState.iaVertexStreamState );
				return baseInterface->queryInterface<TIAVertexStreamState>();
			}
			return nullptr;
		}

		template <typename TIAVertexStreamState = IAVertexStreamImmutableState>
		inline const TIAVertexStreamState & getCurrentIAVertexStreamStateRef() const
		{
			if( const auto * stateObject = getCurrentIAVertexStreamState<TIAVertexStreamState>() )
			{
				return *stateObject;
			}
			ts3Throw( 0 );
		}

		template <typename TRenderTargetBindingState = RenderTargetBindingImmutableState>
		inline const TRenderTargetBindingState * getCurrentRenderTargetBindingState() const noexcept
		{
			if( _currentCommonState.renderTargetBindingState )
			{
				const auto * baseInterface = reinterpret_cast<const GPUAPIObject *>( _currentCommonState.renderTargetBindingState );
				return baseInterface->queryInterface<TRenderTargetBindingState>();
			}
			return nullptr;
		}

		template <typename TRenderTargetBindingState = RenderTargetBindingImmutableState>
		inline const TRenderTargetBindingState & getCurrentRenderTargetBindingStateRef() const
		{
			if( const auto * stateObject = getCurrentRenderTargetBindingState<TRenderTargetBindingState>() )
			{
				return *stateObject;
			}
			ts3Throw( 0 );
		}

	protected:
		void resetStateUpdateMask();

	protected:
		struct CurrentCommonState
		{
			const GraphicsPipelineStateObject * graphicsPSO = nullptr;
			const IAVertexStreamImmutableState * iaVertexStreamState = nullptr;
			const RenderTargetBindingImmutableState * renderTargetBindingState = nullptr;
		};

		CurrentCommonState _currentCommonState;

		Bitmask<graphics_state_update_mask_value_t> _stateUpdateMask = 0;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_CONTROLLER_H__
