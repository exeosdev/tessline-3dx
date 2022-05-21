
#include "separablePipelineState.h"
#include <ts3/gpuapi/state/renderTargetCommon.h>

namespace ts3::gpuapi
{

	SeparableGraphicsPipelineStateObject::SeparableGraphicsPipelineStateObject( GPUDevice & pGPUDevice,
	                                                                            RenderTargetLayout pRenderTargetLayout,
	                                                                            GraphicsShaderBinding pShaderBinding,
	                                                                            ShaderInputSignature pShaderInputSignature,
	                                                                            const SeparableGraphicsStateDescriptorSet & pStateDescriptors )
	: GraphicsPipelineStateObject( pGPUDevice, std::move( pRenderTargetLayout ), std::move( pShaderInputSignature ) )
	, mShaderBinding( std::move( pShaderBinding ) )
	, mSeparableDescriptorSet( pStateDescriptors )
	{}

	SeparableGraphicsPipelineStateObject::~SeparableGraphicsPipelineStateObject() = default;


	SeparableGraphicsPipelineStateController::SeparableGraphicsPipelineStateController()
	: GraphicsPipelineStateController()
	{}

	SeparableGraphicsPipelineStateController::~SeparableGraphicsPipelineStateController() = default;

	bool SeparableGraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject * pGraphicsPipelineSO )
	{
		bool updateResult = GraphicsPipelineStateController::setGraphicsPipelineStateObject( pGraphicsPipelineSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			const auto * separableGPSO = pGraphicsPipelineSO->queryInterface<SeparableGraphicsPipelineStateObject>();

			const auto descriptorsUpdateMask = setGraphicsPSODescriptors( *separableGPSO );

			const auto shadersUpdateMask = setGraphicsPSOShaders( *separableGPSO );

			if( !descriptorsUpdateMask.empty() || !shadersUpdateMask.empty() )
			{
				return true;
			}
		}

		return false;
	}

	bool SeparableGraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject * pVertexStreamSO )
	{
		return GraphicsPipelineStateController::setVertexStreamStateObject( pVertexStreamSO );
	}

	void SeparableGraphicsPipelineStateController::applyPendingPipelineStateChange()
	{
		_currentPipelineConfig.shaderBinding = _pendingPipelineConfig.shaderBinding;
		_currentPipelineConfig.stateDescriptors = _pendingPipelineConfig.stateDescriptors;

		_pendingPipelineConfig.reset();

		GraphicsPipelineStateController::applyPendingPipelineStateChange();
	}

	Bitmask<uint64> SeparableGraphicsPipelineStateController::setGraphicsPSODescriptors( const SeparableGraphicsPipelineStateObject & pSeparableGSPO )
	{
		auto descriptorsUpdateMask = makeBitmask<uint64>( 0 );

		const auto blendStateDescriptorID = pSeparableGSPO.mSeparableDescriptorSet.blendDescriptorID;
		if( blendStateDescriptorID != _currentPipelineConfig.stateDescriptors.blendDescriptorID )
		{
			_pendingPipelineConfig.stateDescriptors.blendDescriptorID = blendStateDescriptorID;
			descriptorsUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_BLEND_BIT );
		}

		const auto depthStencilDescriptorID = pSeparableGSPO.mSeparableDescriptorSet.depthStencilDescriptorID;
		if( depthStencilDescriptorID != _currentPipelineConfig.stateDescriptors.depthStencilDescriptorID )
		{
			_pendingPipelineConfig.stateDescriptors.depthStencilDescriptorID = depthStencilDescriptorID;
			descriptorsUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_DEPTH_STENCIL_BIT );
		}

		const auto rasterizerDescriptorID = pSeparableGSPO.mSeparableDescriptorSet.rasterizerDescriptorID;
		if( rasterizerDescriptorID != _currentPipelineConfig.stateDescriptors.rasterizerDescriptorID )
		{
			_pendingPipelineConfig.stateDescriptors.rasterizerDescriptorID = rasterizerDescriptorID;
			descriptorsUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_RASTERIZER_BIT );
		}

		const auto vertexInputFormatDescriptorID = pSeparableGSPO.mSeparableDescriptorSet.vertexInputFormatDescriptorID;
		if( vertexInputFormatDescriptorID != _currentPipelineConfig.stateDescriptors.vertexInputFormatDescriptorID )
		{
			_pendingPipelineConfig.stateDescriptors.vertexInputFormatDescriptorID = vertexInputFormatDescriptorID;
			descriptorsUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT );
		}

		_stateUpdateMask.set( descriptorsUpdateMask );

		return descriptorsUpdateMask;
	}

	Bitmask<uint64> SeparableGraphicsPipelineStateController::setGraphicsPSOShaders( const SeparableGraphicsPipelineStateObject & pSeparableGSPO )
	{
		auto shadersUpdateMask = makeBitmask<uint64>( 0 );

		auto * vertexShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Vertex];
		if( vertexShader != _currentPipelineConfig.shaderBinding.vertexShader )
		{
			_pendingPipelineConfig.shaderBinding.vertexShader = vertexShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_VERTEX_STAGE_BIT );
		}

		auto * tessControlShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::TessControl];
		if( tessControlShader != _currentPipelineConfig.shaderBinding.tessControlShader )
		{
			_pendingPipelineConfig.shaderBinding.tessControlShader = tessControlShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_TESS_CONTROL_STAGE_BIT );
		}

		auto * tessEvaluationShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::TessEvaluation];
		if( tessEvaluationShader != _currentPipelineConfig.shaderBinding.tessEvaluationShader )
		{
			_pendingPipelineConfig.shaderBinding.tessEvaluationShader = tessEvaluationShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_TESS_EVALUATION_STAGE_BIT );
		}

		auto * geometryShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Geometry];
		if( geometryShader != _currentPipelineConfig.shaderBinding.geometryShader )
		{
			_pendingPipelineConfig.shaderBinding.geometryShader = geometryShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_GEOMETRY_STAGE_BIT );
		}

		auto * amplificationShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Amplification];
		if( amplificationShader != _currentPipelineConfig.shaderBinding.amplificationShader )
		{
			_pendingPipelineConfig.shaderBinding.amplificationShader = amplificationShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_AMPLIFICATION_STAGE_BIT );
		}

		auto * meshShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Mesh];
		if( meshShader != _currentPipelineConfig.shaderBinding.meshShader )
		{
			_pendingPipelineConfig.shaderBinding.meshShader = meshShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_MESH_STAGE_BIT );
		}

		auto * pixelShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Pixel];
		if( pixelShader != _currentPipelineConfig.shaderBinding.pixelShader )
		{
			_pendingPipelineConfig.shaderBinding.pixelShader = pixelShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_PIXEL_STAGE_BIT );
		}

		_stateUpdateMask.set( shadersUpdateMask );

		return shadersUpdateMask;
	}

} // namespace ts3::gpuapi
