
#include "graphicsPipelineStateController.h"
#include "pipelineStateObject.h"
#include "inputAssemblerImmutableStates.h"
#include "renderTargetImmutableStates.h"

#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	GraphicsPipelineStateController::GraphicsPipelineStateController() = default;

	GraphicsPipelineStateController::~GraphicsPipelineStateController() = default;

	void GraphicsPipelineStateController::applyPipelineStateChanges()
	{}

	bool GraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		if( _currentCommonState.graphicsPSO != &pGraphicsPSO )
		{
			_currentCommonState.graphicsPSO = &pGraphicsPSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT );
		}

		return _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT );
	}

	bool GraphicsPipelineStateController::resetGraphicsPipelineStateObject()
	{
		if( _currentCommonState.graphicsPSO )
		{
			_currentCommonState.graphicsPSO = nullptr;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT );
		}

		return _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT );
	}

	bool GraphicsPipelineStateController::setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState )
	{
		_currentCommonState.iaVertexStreamState = &( IAVertexStreamImmutableState::getDynamicOverrideState() );
		_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT );
		return true;
	}

	bool GraphicsPipelineStateController::setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState )
	{
		if( _currentCommonState.iaVertexStreamState != &pIAVertexStreamState )
		{
			_currentCommonState.iaVertexStreamState = &pIAVertexStreamState;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT );
		}

		return _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT );
	}

	bool GraphicsPipelineStateController::resetIAVertexStreamState()
	{
		_currentCommonState.iaVertexStreamState = nullptr;
		_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT );
		return true;
	}

	bool GraphicsPipelineStateController::setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState )
	{
		_currentCommonState.renderTargetBindingState = &( RenderTargetBindingImmutableState::getDynamicOverrideState() );
		_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT );
		return true;
	}

	bool GraphicsPipelineStateController::setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState )
	{
		if( _currentCommonState.renderTargetBindingState != &pRenderTargetBindingState )
		{
			_currentCommonState.renderTargetBindingState = &pRenderTargetBindingState;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT );
		}

		return _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT );
	}

	bool GraphicsPipelineStateController::resetRenderTargetBindingState()
	{
		_currentCommonState.renderTargetBindingState = nullptr;
		_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT );
		return true;
	}

} // namespace ts3::gpuapi
