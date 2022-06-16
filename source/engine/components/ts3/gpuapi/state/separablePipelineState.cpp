
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

	bool SeparableGraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		// Call the base method. This will check the provided SO against the current one (and maybe some extra checks).
		// If this returns false, it means we should skip this call and do not update anything.
		bool updateResult = GraphicsPipelineStateController::setGraphicsPipelineStateObject( pGraphicsPipelineSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			const auto * separableGPSO = pGraphicsPipelineSO.queryInterface<SeparableGraphicsPipelineStateObject>();

			// Update the descriptors. Returned mask is a combination of all E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_*_BIT
			// bits for stages which has been updated. Zero means all descriptors in the PSO match those currently bound.
			const auto descriptorsUpdateMask = setGraphicsPSODescriptors( *separableGPSO );

			// Update the shaders. This mask represents updated shader stages (just like for descriptors).
			const auto shadersUpdateMask = setGraphicsPSOShaders( *separableGPSO );

			if( descriptorsUpdateMask.empty() && shadersUpdateMask.empty() )
			{
				// It is absolutely possible to have two different PSOs that ended up having the same shaders and descriptors:
				// for separable states we use cache to ensure each unique state combination is represented by only one
				// state descriptor - if the config is repeated, that cached object will be returned when a PSO is created.
				// When this happens, we skip setting anything and clear the state. Change result to 'false' to indicate this.
				// TODO: This is a rather unwanted scenario. Maybe some logging here would be accurate?
				updateResult = false;

				// Clear all bits related to separable states.
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SEPARABLE_ALL_BITS_MASK );

				// Clear the bit for graphics SO. There is nothing to set.
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
			}
		}

		return updateResult;
	}

	bool SeparableGraphicsPipelineStateController::resetGraphicsPipelineStateObject()
	{
		return GraphicsPipelineStateController::resetGraphicsPipelineStateObject();
	}

	Bitmask<uint64> SeparableGraphicsPipelineStateController::setGraphicsPSODescriptors( const SeparableGraphicsPipelineStateObject & pSeparableGSPO )
	{
		auto descriptorsUpdateMask = makeBitmask<uint64>( 0 );

		const auto blendStateDescriptorID = pSeparableGSPO.mSeparableDescriptorSet.blendDescriptorID;
		if( blendStateDescriptorID != _currentSeparableStateDescriptors.blendDescriptorID )
		{
			_currentSeparableStateDescriptors.blendDescriptorID = blendStateDescriptorID;
			descriptorsUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_BLEND_BIT );
		}

		const auto depthStencilDescriptorID = pSeparableGSPO.mSeparableDescriptorSet.depthStencilDescriptorID;
		if( depthStencilDescriptorID != _currentSeparableStateDescriptors.depthStencilDescriptorID )
		{
			_currentSeparableStateDescriptors.depthStencilDescriptorID = depthStencilDescriptorID;
			descriptorsUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_DEPTH_STENCIL_BIT );
		}

		const auto rasterizerDescriptorID = pSeparableGSPO.mSeparableDescriptorSet.rasterizerDescriptorID;
		if( rasterizerDescriptorID != _currentSeparableStateDescriptors.rasterizerDescriptorID )
		{
			_currentSeparableStateDescriptors.rasterizerDescriptorID = rasterizerDescriptorID;
			descriptorsUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_RASTERIZER_BIT );
		}

		const auto vertexInputFormatDescriptorID = pSeparableGSPO.mSeparableDescriptorSet.vertexInputFormatDescriptorID;
		if( vertexInputFormatDescriptorID != _currentSeparableStateDescriptors.vertexInputFormatDescriptorID )
		{
			_currentSeparableStateDescriptors.vertexInputFormatDescriptorID = vertexInputFormatDescriptorID;
			descriptorsUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT );
		}

		_stateUpdateMask.set( descriptorsUpdateMask );

		return descriptorsUpdateMask;
	}

	Bitmask<uint64> SeparableGraphicsPipelineStateController::setGraphicsPSOShaders( const SeparableGraphicsPipelineStateObject & pSeparableGSPO )
	{
		auto shadersUpdateMask = makeBitmask<uint64>( 0 );

		auto * vertexShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Vertex];
		if( vertexShader != _currentSeparableShaderBinding.vertexShader )
		{
			_currentSeparableShaderBinding.vertexShader = vertexShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_VERTEX_STAGE_BIT );
		}

		auto * tessControlShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::TessControl];
		if( tessControlShader != _currentSeparableShaderBinding.tessControlShader )
		{
			_currentSeparableShaderBinding.tessControlShader = tessControlShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_TESS_CONTROL_STAGE_BIT );
		}

		auto * tessEvaluationShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::TessEvaluation];
		if( tessEvaluationShader != _currentSeparableShaderBinding.tessEvaluationShader )
		{
			_currentSeparableShaderBinding.tessEvaluationShader = tessEvaluationShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_TESS_EVALUATION_STAGE_BIT );
		}

		auto * geometryShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Geometry];
		if( geometryShader != _currentSeparableShaderBinding.geometryShader )
		{
			_currentSeparableShaderBinding.geometryShader = geometryShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_GEOMETRY_STAGE_BIT );
		}

		auto * amplificationShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Amplification];
		if( amplificationShader != _currentSeparableShaderBinding.amplificationShader )
		{
			_currentSeparableShaderBinding.amplificationShader = amplificationShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_AMPLIFICATION_STAGE_BIT );
		}

		auto * meshShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Mesh];
		if( meshShader != _currentSeparableShaderBinding.meshShader )
		{
			_currentSeparableShaderBinding.meshShader = meshShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_MESH_STAGE_BIT );
		}

		auto * pixelShader = pSeparableGSPO.mShaderBinding[EGraphicsShaderStageID::Pixel];
		if( pixelShader != _currentSeparableShaderBinding.pixelShader )
		{
			_currentSeparableShaderBinding.pixelShader = pixelShader;
			shadersUpdateMask.set( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_PIXEL_STAGE_BIT );
		}

		_stateUpdateMask.set( shadersUpdateMask );

		return shadersUpdateMask;
	}

} // namespace ts3::gpuapi
