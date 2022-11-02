
#include "graphicsPipelineStateController.h"
#include "pipelineStateObject.h"
#include "vertexStreamStateObject.h"
#include <ts3/stdext/memory.h>

namespace ts3::GpuAPI
{

	GraphicsPipelineStateController::GraphicsPipelineStateController() = default;

	GraphicsPipelineStateController::~GraphicsPipelineStateController() = default;

	bool GraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		if( _currentGraphicsPipelineSO != &pGraphicsPipelineSO )
		{
			_currentGraphicsPipelineSO = &pGraphicsPipelineSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
		}

		return _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
	}

	bool GraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		if( _currentVertexStreamSO != &pVertexStreamSO )
		{
			_currentVertexStreamSO = &pVertexStreamSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
		}

		return _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
	}

	bool GraphicsPipelineStateController::resetGraphicsPipelineStateObject()
	{
		if( _currentGraphicsPipelineSO )
		{
			_currentGraphicsPipelineSO = nullptr;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
		}

		return _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
	}

	bool GraphicsPipelineStateController::resetVertexStreamStateObject()
	{
		if( _currentVertexStreamSO )
		{
			_currentVertexStreamSO = nullptr;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
		}

		return _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
	}

} // namespace ts3::GpuAPI
