
#include "DX11_pipelineStateController.h"
#include "DX11_pipelineStateObject.h"
#include "DX11_commonGraphicsConfig.h"
#include "DX11_inputAssembler.h"
#include "DX11_graphicsShaderState.h"
#include "DX11_renderTarget.h"
#include <ts3/gpuapiDX11/DX11_commandList.h>
#include <ts3/gpuapiDX11/resources/DX11_shader.h>

namespace ts3::gpuapi
{

	DX11GraphicsPipelineStateController::DX11GraphicsPipelineStateController( DX11CommandList & pDX11CommandList )
	: GraphicsPipelineStateControllerSeparableShader()
	, mDX11CommandList( &pDX11CommandList )
	{}

	DX11GraphicsPipelineStateController::~DX11GraphicsPipelineStateController() = default;

	bool DX11GraphicsPipelineStateController::applyStateChanges()
	{
		Bitmask<uint32> executedUpdatesMask = 0;

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT ) )
		{
			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_STATES_ALL ) )
			{
				auto * dx11GraphicsPSO = _currentCommonState.graphicsPSO->queryInterface<DX11GraphicsPipelineStateObject>();
				const auto commonPSOUpdateMask = applyCommonGraphicsConfigState( *dx11GraphicsPSO );
				executedUpdatesMask.set( commonPSOUpdateMask );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT ) )
			{
				const auto & separableShaderCache = getCurrentSeparableShaders();
				const auto shaderUpdateMask = applyGraphicsShaderState( separableShaderCache );
				executedUpdatesMask.set( shaderUpdateMask );
			}
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT ) )
		{
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT ) )
		{
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT );
		}

		return !executedUpdatesMask.empty();
	}

	bool DX11GraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		return GraphicsPipelineStateControllerSeparableShader::setGraphicsPipelineStateObject( pGraphicsPSO );
	}

	bool DX11GraphicsPipelineStateController::resetGraphicsPipelineStateObject()
	{
		return GraphicsPipelineStateControllerSeparableShader::resetGraphicsPipelineStateObject();
	}

	bool DX11GraphicsPipelineStateController::setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState )
	{
		bool updateResult = GraphicsPipelineStateControllerSeparableShader::setIAVertexStreamState( pIAVertexStreamState );

		if( updateResult )
		{
			// translate dynamic Vertex Stream state
		}

		return updateResult;
	}

	bool DX11GraphicsPipelineStateController::setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState )
	{
		return GraphicsPipelineStateControllerSeparableShader::setIAVertexStreamState( pIAVertexStreamState );
	}

	bool DX11GraphicsPipelineStateController::resetIAVertexStreamState()
	{
		return GraphicsPipelineStateControllerSeparableShader::resetIAVertexStreamState();
	}

	bool DX11GraphicsPipelineStateController::setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState )
	{
		bool updateResult = GraphicsPipelineStateControllerSeparableShader::setRenderTargetBindingState( pRenderTargetBindingState );

		if( updateResult )
		{
			// translate dynamic Render Target Binding Stream state
		}

		return updateResult;
	}

	bool DX11GraphicsPipelineStateController::setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState )
	{
		return GraphicsPipelineStateControllerSeparableShader::setRenderTargetBindingState( pRenderTargetBindingState );
	}

	bool DX11GraphicsPipelineStateController::resetRenderTargetBindingState()
	{
		return GraphicsPipelineStateControllerSeparableShader::resetRenderTargetBindingState();
	}

	bool DX11GraphicsPipelineStateController::setViewport( const ViewportDesc & pViewportDesc )
	{
	}

	bool DX11GraphicsPipelineStateController::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
	}

	bool DX11GraphicsPipelineStateController::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
	}

	bool DX11GraphicsPipelineStateController::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
	}

	bool DX11GraphicsPipelineStateController::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
	}

	Bitmask<uint32> DX11GraphicsPipelineStateController::applyCommonGraphicsConfigState( const DX11GraphicsPipelineStateObject & pGraphicsPSO )
	{
		Bitmask<uint32> executedUpdatesMask = 0;

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT ) )
		{
			const auto & blendState = pGraphicsPSO.getBlendState();
			const auto & dynamicState = getRenderPassDynamicState();

			if( dynamicState.activeStateMask.isSet( E_GRAPHICS_PIPELINE_DYNAMIC_STATE_FLAG_BLEND_CONSTANT_COLOR_BIT ) )
			{
				mDX11CommandList->mD3D11DeviceContext1->OMSetBlendState(
						blendState.mD3D11BlendState.Get(),
						dynamicState.blendConstantColor.rgbaArray,
						0xFFFFFFFF );
			}
			else
			{
				mDX11CommandList->mD3D11DeviceContext1->OMSetBlendState(
						blendState.mD3D11BlendState.Get(),
						nullptr,
						0xFFFFFFFF );
			}

			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT ) )
		{
			const auto & depthStencilState = pGraphicsPSO.getDepthStencilState();
			const auto & dynamicState = getRenderPassDynamicState();

			if( dynamicState.activeStateMask.isSet( E_GRAPHICS_PIPELINE_DYNAMIC_STATE_FLAG_STENCIL_REF_VALUE_BIT ) )
			{
				mDX11CommandList->mD3D11DeviceContext1->OMSetDepthStencilState(
						depthStencilState.mD3D11DepthStencilState.Get(),
						dynamicState.stencilTestRefValue );
			}
			else
			{
				mDX11CommandList->mD3D11DeviceContext1->OMSetDepthStencilState(
						depthStencilState.mD3D11DepthStencilState.Get(),
						0x00 );
			}

			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT ) )
		{
			const auto & rasterizerState = pGraphicsPSO.getRasterizerState();
			const auto & dynamicState = getRenderPassDynamicState();

			mDX11CommandList->mD3D11DeviceContext1->RSSetState( rasterizerState.mD3D11RasterizerState.Get() );

			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT ) )
		{
			const auto & iaInputLayoutState = pGraphicsPSO.getIAInputLayoutState();
			const auto & dynamicState = getRenderPassDynamicState();

			mDX11CommandList->mD3D11DeviceContext1->IASetPrimitiveTopology( iaInputLayoutState.mD3D11PrimitiveTopology );
			mDX11CommandList->mD3D11DeviceContext1->IASetInputLayout( iaInputLayoutState.mD3D11InputLayout.Get() );

			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT );
		}

		return executedUpdatesMask;
	}

	Bitmask<uint32> DX11GraphicsPipelineStateController::applyGraphicsShaderState( const SeparableShaderCache & pSeparableShaders )
	{
		Bitmask<uint32> executedUpdatesMask = 0;

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_VERTEX_BIT ) )
		{
			auto * dx11Shader = pSeparableShaders.vertexShader->queryInterface<DX11Shader>();
			mDX11CommandList->mD3D11DeviceContext1->VSSetShader( dx11Shader->mD3D11VertexShader, nullptr, 0 );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_VERTEX_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_HULL_BIT ) )
		{
			auto * dx11Shader = pSeparableShaders.hullShader->queryInterface<DX11Shader>();
			mDX11CommandList->mD3D11DeviceContext1->HSSetShader( dx11Shader->mD3D11HullShader, nullptr, 0 );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_HULL_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_DOMAIN_BIT ) )
		{
			auto * dx11Shader = pSeparableShaders.domainShader->queryInterface<DX11Shader>();
			mDX11CommandList->mD3D11DeviceContext1->DSSetShader( dx11Shader->mD3D11DomainShader, nullptr, 0 );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_DOMAIN_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_GEOMETRY_BIT ) )
		{
			auto * dx11Shader = pSeparableShaders.geometryShader->queryInterface<DX11Shader>();
			mDX11CommandList->mD3D11DeviceContext1->GSSetShader( dx11Shader->mD3D11GeometryShader, nullptr, 0 );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_GEOMETRY_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_PIXEL_BIT ) )
		{
			auto * dx11Shader = pSeparableShaders.pixelShader->queryInterface<DX11Shader>();
			mDX11CommandList->mD3D11DeviceContext1->PSSetShader( dx11Shader->mD3D11PixelShader, nullptr, 0 );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_SHADER_PIXEL_BIT );
		}

		return executedUpdatesMask;
	}

	void DX11GraphicsPipelineStateController::applyIAVertexStreamState( const DX11IAVertexStreamImmutableState & pVertexStreamState )
	{
		const auto & indexBufferBinding = pVertexStreamState.mDX11VertexStreamDefinition.indexBufferBinding;
		const auto & vertexBufferBindings = pVertexStreamState.mDX11VertexStreamDefinition.vertexBufferBindings;

		mDX11CommandList->mD3D11DeviceContext1->IASetIndexBuffer(
				indexBufferBinding.buffer,
				indexBufferBinding.format,
				indexBufferBinding.offset );

		for( const auto & vertexStreamRange : vertexBufferBindings.activeRanges )
		{
			mDX11CommandList->mD3D11DeviceContext1->IASetVertexBuffers(
					vertexStreamRange.firstIndex,
					vertexStreamRange.length,
					&( vertexBufferBindings.bindingData.bufferArray[vertexStreamRange.firstIndex] ),
					&( vertexBufferBindings.bindingData.strideArray[vertexStreamRange.firstIndex] ),
					&( vertexBufferBindings.bindingData.offsetArray[vertexStreamRange.firstIndex] ) );
		}
	}

	bool DX11GraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		return SeparableGraphicsPipelineStateController::setGraphicsPipelineStateObject( pGraphicsPipelineSO );
	}

	bool DX11GraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		return SeparableGraphicsPipelineStateController::setVertexStreamStateObject( pVertexStreamSO );
	}

	bool DX11GraphicsPipelineStateController::updatePipelineState()
	{
		if( !SeparableGraphicsPipelineStateController::updatePipelineState() )
		{
			return false;
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_BLEND_BIT ) )
			{
				auto blendDescriptorID = _separableStateDescriptorSet.blendDescriptorID;
				const auto & blendDescriptor = _descriptorCache->getBlendDescriptor( blendDescriptorID );

				mDX11CommandList->mD3D11DeviceContext1->OMSetBlendState( blendDescriptor.d3d11BlendState.Get(),
				                                                         blendDescriptor.configDesc.ConstantFactor.rgbaArray,
				                                                         0xFFFFFFFF );

				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_BLEND_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_DEPTH_STENCIL_BIT ) )
			{
				auto depthStencilDescriptorID = _separableStateDescriptorSet.depthStencilDescriptorID;
				const auto & depthStencilDescriptor = _descriptorCache->getDepthStencilDescriptor( depthStencilDescriptorID );

				mDX11CommandList->mD3D11DeviceContext1->OMSetDepthStencilState( depthStencilDescriptor.d3d11DepthStencilState.Get(),
				                                                                depthStencilDescriptor.configDesc.StencilRefValue );

				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_DEPTH_STENCIL_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_RASTERIZER_BIT ) )
			{
				auto rasterizerDescriptorID = _separableStateDescriptorSet.rasterizerDescriptorID;
				const auto & rasterizerDescriptor = _descriptorCache->getRasterizerDescriptor( rasterizerDescriptorID );

				mDX11CommandList->mD3D11DeviceContext1->RSSetState( rasterizerDescriptor.d3d11RasterizerState.Get() );

				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_RASTERIZER_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT ) )
			{
				auto vertexInputFormatDescriptorID = _separableStateDescriptorSet.vertexInputFormatDescriptorID;
				const auto & vertexInputFormatDescriptor = _descriptorCache->getVertexInputFormatDescriptor( vertexInputFormatDescriptorID );

				mDX11CommandList->mD3D11DeviceContext1->IASetPrimitiveTopology( vertexInputFormatDescriptor.inputFormatDesc.primitiveTopology );
				mDX11CommandList->mD3D11DeviceContext1->IASetInputLayout( vertexInputFormatDescriptor.d3d11InputLayout.Get() );

				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_STAGE_BIT ) )
			{
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_VERTEX_STAGE_BIT ) )
				{
					auto * dx11Shader = _separableShaderBinding.vertexShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->VSSetShader( dx11Shader->mD3D11VertexShader, nullptr, 0 );
				}
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_TESS_CONTROL_STAGE_BIT ) )
				{
					auto * dx11Shader = _separableShaderBinding.tessControlShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->HSSetShader( dx11Shader->mD3D11HullShader, nullptr, 0 );
				}
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_TESS_EVALUATION_STAGE_BIT ) )
				{
					auto * dx11Shader = _separableShaderBinding.tessEvaluationShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->DSSetShader( dx11Shader->mD3D11DomainShader, nullptr, 0 );
				}
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_GEOMETRY_STAGE_BIT ) )
				{
					auto * dx11Shader = _separableShaderBinding.geometryShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->GSSetShader( dx11Shader->mD3D11GeometryShader, nullptr, 0 );
				}
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_PIXEL_STAGE_BIT ) )
				{
					auto * dx11Shader = _separableShaderBinding.pixelShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->PSSetShader( dx11Shader->mD3D11PixelShader, nullptr, 0 );
				}
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ALL_BITS_MASK );
			}

			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT ) )
		{
			auto * openglVertexStreamSO = _csCommonConfig.soVertexStream->queryInterface<DX11VertexStreamStateObject>();

			const auto & indexBufferBinding = openglVertexStreamSO->mDX11VertexDataSourceBinding.indexBufferBinding;
			const auto & vertexBufferBinding = openglVertexStreamSO->mDX11VertexDataSourceBinding.vertexBufferBinding;

			mDX11CommandList->mD3D11DeviceContext1->IASetIndexBuffer( indexBufferBinding.buffer,
			                                                          indexBufferBinding.format,
			                                                          indexBufferBinding.offset );

			for( const auto & vertexStreamRange : openglVertexStreamSO->mDX11VertexDataSourceBinding.vertexStreamActiveRangeList )
			{
				mDX11CommandList->mD3D11DeviceContext1->IASetVertexBuffers( vertexStreamRange.firstIndex,
				                                                            vertexStreamRange.length,
				                                                            &( vertexBufferBinding.bufferArray[vertexStreamRange.firstIndex] ),
				                                                            &( vertexBufferBinding.strideArray[vertexStreamRange.firstIndex] ),
				                                                            &( vertexBufferBinding.offsetArray[vertexStreamRange.firstIndex] ) );
			}

			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
		}

		return _stateUpdateMask != 0;
	}

} // namespace ts3::gpuapi
