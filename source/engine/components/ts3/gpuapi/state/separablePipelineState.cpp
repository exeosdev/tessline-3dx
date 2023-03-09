
#include "separablePipelineState.h"
#include <ts3/gpuapi/state/renderTargetCommon.h>

namespace ts3::gpuapi
{

	GraphicsPipelineStateControllerSeparable::GraphicsPipelineStateControllerSeparable() = default;

	GraphicsPipelineStateControllerSeparable::~GraphicsPipelineStateControllerSeparable() = default;

	const SeparablePSOStateSet & GraphicsPipelineStateControllerSeparable::getCurrentSeparableStates() const noexcept
	{
		return _currentSeparableStates;
	}

	bool GraphicsPipelineStateControllerSeparable::setGraphicsPipelineStateObject(
			const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		bool updateResult = GraphicsPipelineStateController::setGraphicsPipelineStateObject( pGraphicsPSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT ) )
		{
			const auto * graphicsPSOSeparable = pGraphicsPSO.queryInterface<GraphicsPipelineStateObjectSeparable>();

			// Update the individual states. Returned mask is a combination of all E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_*_BIT
			// bits for states which has been updated. Zero means all states in the PSO match those currently bound.
			const auto stateUpdateMask = setSeparablePSOStates( *graphicsPSOSeparable );

			if( stateUpdateMask.empty() )
			{
				// It is absolutely possible to have two different PSOs that ended up having the same shaders and pdesc:
				// for separable states we use cache to ensure each unique state combination is represented by only one
				// state descriptor - if the config is repeated, that cached object will be returned when a PSO is created.
				// When this happens, we skip setting anything and clear the state. Change result to 'false' to indicate this.
				// TODO: This is a rather unwanted scenario. Maybe some logging here would be accurate?
				updateResult = false;

				// Clear the bit for the graphics PSO itself. There is nothing to set.
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT );
			}
		}

		return updateResult;
	}

	bool GraphicsPipelineStateControllerSeparable::resetGraphicsPipelineStateObject()
	{
		return GraphicsPipelineStateController::resetGraphicsPipelineStateObject();
	}

	Bitmask<uint64> GraphicsPipelineStateControllerSeparable::setSeparablePSOStates(
			const GraphicsPipelineStateObjectSeparable & pGraphicsPSOSeparable )
	{
		auto stateUpdateMask = makeBitmask<uint64>( 0 );

		const auto * blendState = pGraphicsPSOSeparable.mSeparableStates.blendState;
		if( blendState != _currentSeparableStates.blendState )
		{
			_currentSeparableStates.blendState = blendState;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT );
		}

		const auto * depthStencilState = pGraphicsPSOSeparable.mSeparableStates.depthStencilState;
		if( depthStencilState != _currentSeparableStates.depthStencilState )
		{
			_currentSeparableStates.depthStencilState = depthStencilState;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT );
		}

		const auto * rasterizerState = pGraphicsPSOSeparable.mSeparableStates.rasterizerState;
		if( rasterizerState != _currentSeparableStates.rasterizerState )
		{
			_currentSeparableStates.rasterizerState = rasterizerState;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT );
		}

		const auto * iaInputLayoutState = pGraphicsPSOSeparable.mSeparableStates.iaInputLayoutState;
		if( iaInputLayoutState != _currentSeparableStates.iaInputLayoutState )
		{
			_currentSeparableStates.iaInputLayoutState = iaInputLayoutState;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT );
		}

		const auto * shaderLinkageState = pGraphicsPSOSeparable.mSeparableStates.shaderLinkageState;
		if( shaderLinkageState != _currentSeparableStates.shaderLinkageState )
		{
			_currentSeparableStates.shaderLinkageState = shaderLinkageState;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT );
		}

		_stateUpdateMask.set( stateUpdateMask );

		return stateUpdateMask;
	}


	GraphicsPipelineStateControllerSeparableShader::GraphicsPipelineStateControllerSeparableShader() = default;

	GraphicsPipelineStateControllerSeparableShader::~GraphicsPipelineStateControllerSeparableShader() = default;

	const SeparableShaderSet & GraphicsPipelineStateControllerSeparableShader::getCurrentSeparableShaders() const noexcept
	{
		return _currentSeparableShaders;
	}

	bool GraphicsPipelineStateControllerSeparableShader::setGraphicsPipelineStateObject(
			const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		bool updateResult = GraphicsPipelineStateControllerSeparable::setGraphicsPipelineStateObject( pGraphicsPSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT ) )
		{
			const auto * graphicsPSOSeparableShader = pGraphicsPSO.queryInterface<GraphicsPipelineStateObjectSeparableShader>();

			// Update the individual states. Returned mask is a combination of all E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_*_BIT
			// bits for states which has been updated. Zero means all states in the PSO match those currently bound.
			const auto stateUpdateMask = setSeparableShaders( *graphicsPSOSeparableShader );

			if( stateUpdateMask.empty() )
			{
				updateResult = false;

				// Clear all bits related to all separable shader stages.
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_SHADERS_ALL );

				// Clear the bit for shader linkage state.
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT );
			}
		}

		return updateResult;
	}

	bool GraphicsPipelineStateControllerSeparableShader::resetGraphicsPipelineStateObject()
	{
		return GraphicsPipelineStateControllerSeparable::resetGraphicsPipelineStateObject();
	}

	Bitmask<uint64> GraphicsPipelineStateControllerSeparableShader::setSeparableShaders(
			const GraphicsPipelineStateObjectSeparableShader & pGraphicsPSOSeparableShader )
	{
		auto stateUpdateMask = makeBitmask<uint64>( 0 );

		auto * vertexShader = pGraphicsPSOSeparableShader.mSeparableShaders[EShaderType::GSVertex];
		if( vertexShader != _currentSeparableShaders.vertexShader )
		{
			_currentSeparableShaders.vertexShader = vertexShader;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_VERTEX_BIT );
		}

		auto * hullShader = pGraphicsPSOSeparableShader.mSeparableShaders[EShaderType::GSHull];
		if( hullShader != _currentSeparableShaders.hullShader )
		{
			_currentSeparableShaders.hullShader = hullShader;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_HULL_BIT );
		}

		auto * domainShader = pGraphicsPSOSeparableShader.mSeparableShaders[EShaderType::GSDomain];
		if( domainShader != _currentSeparableShaders.domainShader )
		{
			_currentSeparableShaders.domainShader = domainShader;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_DOMAIN_BIT );
		}

		auto * geometryShader = pGraphicsPSOSeparableShader.mSeparableShaders[EShaderType::GSGeometry];
		if( geometryShader != _currentSeparableShaders.geometryShader )
		{
			_currentSeparableShaders.geometryShader = geometryShader;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_GEOMETRY_BIT );
		}

		auto * pixelShader = pGraphicsPSOSeparableShader.mSeparableShaders[EShaderType::GSPixel];
		if( pixelShader != _currentSeparableShaders.pixelShader )
		{
			_currentSeparableShaders.pixelShader = pixelShader;
			stateUpdateMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_PIXEL_BIT );
		}

		_stateUpdateMask.set( stateUpdateMask );

		return stateUpdateMask;
	}

} // namespace ts3::gpuapi
