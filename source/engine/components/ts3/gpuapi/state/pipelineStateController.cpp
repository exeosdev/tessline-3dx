
#include "pipelineStateController.h"
#include "pipelineStateObject.h"
#include "vertexStreamStateObject.h"
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	GraphicsPipelineStateController::GraphicsPipelineStateController() = default;

	GraphicsPipelineStateController::~GraphicsPipelineStateController() = default;

	void GraphicsPipelineStateController::applyPendingPipelineStateChange()
	{
		_currentStateObjects.graphicsPipelineSO = _pendingStateObjects.graphicsPipelineSO;
		_currentStateObjects.vertexStreamSO = _pendingStateObjects.vertexStreamSO;

		_pendingStateObjects.reset();
		_stateUpdateMask.clear();
	}

	bool GraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject * pGraphicsPipelineSO )
	{
		bool updateResult = false;

		if( pGraphicsPipelineSO != _currentStateObjects.graphicsPipelineSO )
		{
			_pendingStateObjects.graphicsPipelineSO = pGraphicsPipelineSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );

			updateResult = true;
		}
		else
		{
			_pendingStateObjects.graphicsPipelineSO = nullptr;
			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
		}

		return updateResult;
	}

	bool GraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject * pVertexStreamSO )
	{
		bool updateResult = false;

		if( pVertexStreamSO != _currentStateObjects.vertexStreamSO )
		{
			_pendingStateObjects.vertexStreamSO = pVertexStreamSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );

			updateResult = true;
		}
		else
		{
			_pendingStateObjects.vertexStreamSO = nullptr;
			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
		}

		return updateResult;
	}

} // namespace ts3::gpuapi
