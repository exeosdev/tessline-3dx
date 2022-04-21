
#include "pipelineStateController.h"
#include "pipelineStateObject.h"
#include "vertexStreamStateObject.h"
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	GraphicsPipelineStateController::GraphicsPipelineStateController()
	{}

	GraphicsPipelineStateController::~GraphicsPipelineStateController() = default;

	bool GraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		bool updateResult = false;

		if( &pGraphicsPipelineSO != _csCommonConfig.soGraphicsPipeline )
		{
			_csCommonConfig.soGraphicsPipeline = &pGraphicsPipelineSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
			updateResult = true;
		}

		return updateResult;
	}

	bool GraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		bool updateResult = false;

		if( &pVertexStreamSO != _csCommonConfig.soVertexStream )
		{
			_csCommonConfig.soVertexStream = &pVertexStreamSO;
			_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
			updateResult = true;
		}

		return updateResult;
	}

	void GraphicsPipelineStateController::resetInternalState()
	{
		_csCommonConfig.soGraphicsPipeline = nullptr;
		_csCommonConfig.soVertexStream = nullptr;
	}

	bool GraphicsPipelineStateController::updatePipelineState()
	{
		return _stateUpdateMask != 0;
	}


	SeparableGraphicsPipelineStateController::SeparableGraphicsPipelineStateController()
	: GraphicsPipelineStateController()
	{}

	SeparableGraphicsPipelineStateController::~SeparableGraphicsPipelineStateController() = default;

	bool SeparableGraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		bool updateResult = GraphicsPipelineStateController::setGraphicsPipelineStateObject( pGraphicsPipelineSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			auto * separableGraphicsPipelineSO = _csCommonConfig.soGraphicsPipeline->queryInterface<SeparableGraphicsPipelineStateObject>();

			auto blendStateDescriptorID = separableGraphicsPipelineSO->mSeparableDescriptorSet.blendDescriptorID;
			if( blendStateDescriptorID != _csSeparableStateDescriptorSet.blendDescriptorID )
			{
				_csSeparableStateDescriptorSet.blendDescriptorID = blendStateDescriptorID;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_BLEND_BIT );
			}

			auto depthStencilDescriptorID = separableGraphicsPipelineSO->mSeparableDescriptorSet.depthStencilDescriptorID;
			if( depthStencilDescriptorID != _csSeparableStateDescriptorSet.depthStencilDescriptorID )
			{
				_csSeparableStateDescriptorSet.depthStencilDescriptorID = depthStencilDescriptorID;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_DEPTH_STENCIL_BIT );
			}

			auto rasterizerDescriptorID = separableGraphicsPipelineSO->mSeparableDescriptorSet.rasterizerDescriptorID;
			if( rasterizerDescriptorID != _csSeparableStateDescriptorSet.rasterizerDescriptorID )
			{
				_csSeparableStateDescriptorSet.rasterizerDescriptorID = rasterizerDescriptorID;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_RASTERIZER_BIT );
			}

			auto vertexInputFormatDescriptorID = separableGraphicsPipelineSO->mSeparableDescriptorSet.vertexInputFormatDescriptorID;
			if( vertexInputFormatDescriptorID != _csSeparableStateDescriptorSet.vertexInputFormatDescriptorID )
			{
				_csSeparableStateDescriptorSet.vertexInputFormatDescriptorID = vertexInputFormatDescriptorID;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT );
			}

			auto * vertexShader = separableGraphicsPipelineSO->mShaderBinding[E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX];
			if( vertexShader != _csSeparableShaderBinding.vertexShader )
			{
				_csSeparableShaderBinding.vertexShader = vertexShader;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SHADER_VERTEX_STAGE_BIT );
			}

			auto * tessControlShader = separableGraphicsPipelineSO->mShaderBinding[E_SHADER_STAGE_INDEX_GRAPHICS_TESS_CONTROL];
			if( tessControlShader != _csSeparableShaderBinding.tessControlShader )
			{
				_csSeparableShaderBinding.tessControlShader = tessControlShader;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SHADER_TESS_CONTROL_STAGE_BIT );
			}

			auto * tessEvaluationShader = separableGraphicsPipelineSO->mShaderBinding[E_SHADER_STAGE_INDEX_GRAPHICS_TESS_EVALUATION];
			if( tessEvaluationShader != _csSeparableShaderBinding.tessEvaluationShader )
			{
				_csSeparableShaderBinding.tessEvaluationShader = tessEvaluationShader;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SHADER_TESS_EVALUATION_STAGE_BIT );
			}

			auto * geometryShader = separableGraphicsPipelineSO->mShaderBinding[E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY];
			if( geometryShader != _csSeparableShaderBinding.geometryShader )
			{
				_csSeparableShaderBinding.geometryShader = geometryShader;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SHADER_GEOMETRY_STAGE_BIT );
			}

			auto * pixelShader = separableGraphicsPipelineSO->mShaderBinding[E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL];
			if( pixelShader != _csSeparableShaderBinding.pixelShader )
			{
				_csSeparableShaderBinding.pixelShader = pixelShader;
				_stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SHADER_PIXEL_STAGE_BIT );
			}
		}

		return updateResult;
	}

	void SeparableGraphicsPipelineStateController::resetInternalState()
	{
		GraphicsPipelineStateController::resetInternalState();

		memZero( _csSeparableShaderBinding );
		memZero( _csSeparableStateDescriptorSet );
	}

} // namespace ts3::gpuapi
