
#include "pipelineStateController.h"
#include "pipelineStateObject.h"
#include "vertexStreamStateObject.h"
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	GraphicsPipelineStateController::GraphicsPipelineStateController() = default;

	GraphicsPipelineStateController::~GraphicsPipelineStateController() = default;

	bool GraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject * pGraphicsPipelineSO )
	{
		bool updateResult = false;

		if( pGraphicsPipelineSO )
		{
			// If the specified PSO pointer is not NULL, use the dedicated method for setting a valid PSO.
			updateResult = setGraphicsPipelineStateObjectInternal( *pGraphicsPipelineSO );
		}
		else
		{
			// If NULL has been specified, use the resetting function.
			updateResult = resetGraphicsPipelineStateObjectInternal();
		}

		if( updateResult )
		{
			// True is returned if anything at all has been changed in the pipeline configuration.
			// Update the internal PSO pointer only if that is the case.
			_graphicsPipelineStateObject = pGraphicsPipelineSO;
		}

		return updateResult;
	}

	bool GraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject * pVertexStreamSO )
	{
		bool updateResult = false;

		if( pVertexStreamSO )
		{
			updateResult = setVertexStreamStateObjectInternal( *pVertexStreamSO );
		}
		else
		{
			updateResult = resetVertexStreamStateObjectInternal();
		}

		if( updateResult )
		{
			_vertexStreamStateObject = pVertexStreamSO;
		}

		return updateResult;
	}

	void GraphicsPipelineStateController::resetInternalState()
	{
		resetGraphicsPipelineStateObjectInternal();
		resetVertexStreamStateObjectInternal();
	}

	bool GraphicsPipelineStateController::updatePipelineState()
	{
		return _stateUpdateMask != 0;
	}

	bool GraphicsPipelineStateController::setGraphicsPipelineStateObjectInternal( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		bool updateResult = false;

		// Mark the PSO as changed only if the one is indeed different from the one currently set.
		if( &pGraphicsPipelineSO != _graphicsPipelineStateObject )
		{
			// Save the new PSO, change the update mask.
			// THis will
			_graphicsPipelineStateObject = &pGraphicsPipelineSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
			updateResult = true;
		}

		return updateResult;
	}

	bool GraphicsPipelineStateController::setVertexStreamStateObjectInternal( const VertexStreamStateObject & pVertexStreamSO )
	{
		bool updateResult = false;

		if( &pVertexStreamSO != _vertexStreamStateObject )
		{
			_vertexStreamStateObject = &pVertexStreamSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
			updateResult = true;
		}

		return updateResult;
	}

} // namespace ts3::gpuapi
