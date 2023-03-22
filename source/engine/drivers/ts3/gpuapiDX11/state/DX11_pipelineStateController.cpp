
#include "DX11_pipelineStateController.h"
#include "DX11_pipelineStateObject.h"
#include "DX11_commonGraphicsConfig.h"
#include "DX11_inputAssembler.h"
#include "DX11_graphicsShaderState.h"
#include "DX11_renderTarget.h"
#include "../DX11_commandList.h"
#include "../resources/DX11_gpuBuffer.h"
#include "../resources/DX11_sampler.h"
#include "../resources/DX11_shader.h"
#include "../resources/DX11_texture.h"

#include <ts3/gpuapi/state/inputAssemblerDynamicStates.h>
#include <ts3/gpuapi/state/renderTargetDynamicStates.h>

namespace ts3::gpuapi
{

	DX11GraphicsPipelineStateController::DX11GraphicsPipelineStateController( DX11CommandList & pDX11CommandList )
	: GraphicsPipelineStateControllerSeparableShader()
	, mGPUDevice( pDX11CommandList.mGPUDevice )
	, mDX11CommandList( &pDX11CommandList )
	, mD3D11DeviceContext1( pDX11CommandList.mD3D11DeviceContext1.Get() )
	{}

	DX11GraphicsPipelineStateController::~DX11GraphicsPipelineStateController() = default;

	const DX11IAVertexStreamDefinition & DX11GraphicsPipelineStateController::getCurrentIAVertexStreamDefinition() const noexcept
	{
		if( isIAVertexStreamStateDynamic() )
		{
			return _dynamicIAVertexStreamDefinitionDX11;
		}
		else
		{
			const auto * dx11VertexStreamState = _currentCommonState.iaVertexStreamState->queryInterface<DX11IAVertexStreamImmutableState>();
			return dx11VertexStreamState->mDX11VertexStreamDefinition;
		}
	}

	DX11RenderTargetBindingData DX11GraphicsPipelineStateController::getCurrentRenderTargetBinding() const noexcept
	{
		if( isRenderTargetStateDynamic() )
		{
			return _dynamicRenderTargetBindingDataDX11;
		}
		else
		{
			const auto * dx11RTBindingState = _currentCommonState.renderTargetBindingState->queryInterface<DX11RenderTargetBindingImmutableState>();
			return dx11RTBindingState->mDX11RTBindingData;
		}
	}

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
			const auto & currentVertexStreamDefinition = getCurrentIAVertexStreamDefinition();
			applyIAVertexStreamState( currentVertexStreamDefinition );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_VERTEX_STREAM_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT ) )
		{
			const auto & currentRenderTargetBinding = getCurrentRenderTargetBinding();
			applyRenderTargetBinding( currentRenderTargetBinding );
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
			auto dx11VertexStreamDefinition =
					smutil::translateIAVertexStreamDefinitionDX11( pIAVertexStreamState.getVertexStreamDefinition() );

			_dynamicIAVertexStreamDefinitionDX11 = dx11VertexStreamDefinition;
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
			auto dx11RenderTargetBinding =
					smutil::createRenderTargetBindingDataDX11( *mGPUDevice.queryInterface<DX11GPUDevice>(), pRenderTargetBindingState.getBindingDefinition() );

			_dynamicRenderTargetBindingDataDX11 = dx11RenderTargetBinding;
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
		D3D11_VIEWPORT viewportDesc;
		viewportDesc.TopLeftX = static_cast<float>( pViewportDesc.origin.x );
		viewportDesc.TopLeftY = static_cast<float>( pViewportDesc.origin.y );
		viewportDesc.Width = static_cast<float>( pViewportDesc.size.x );
		viewportDesc.Height = static_cast<float>( pViewportDesc.size.y );
		viewportDesc.MinDepth = pViewportDesc.depthRange.zNear;
		viewportDesc.MaxDepth = pViewportDesc.depthRange.zFar;

		mDX11CommandList->mD3D11DeviceContext1->RSSetViewports( 1, &viewportDesc );

		return true;
	}

	bool DX11GraphicsPipelineStateController::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		return false;
	}

	bool DX11GraphicsPipelineStateController::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		const auto & shaderInputSignature = getShaderInputSignature();
		const auto & descriptorInfo = shaderInputSignature.getDescriptorInfo( pParamRefID );

		ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Resource );
		ts3DebugAssert( descriptorInfo.uResourceInfo.resourceType == EShaderInputResourceType::CBVConstantBuffer );

		auto updatedStagesMask = makeBitmask<EShaderStageFlags>();

		if( descriptorInfo.dShaderVisibilityMask != 0 )
		{
			auto * dx11Buffer = pConstantBuffer.queryInterface<DX11GPUBuffer>();
			auto * d3d11Buffer = dx11Buffer->mD3D11Buffer.Get();

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT ) )
			{
				mD3D11DeviceContext1->VSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT) )
			{
				mD3D11DeviceContext1->HSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT ) )
			{
				mD3D11DeviceContext1->DSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT ) )
			{
				mD3D11DeviceContext1->GSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT ) )
			{
				mD3D11DeviceContext1->PSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT );
			}
		}

		return !updatedStagesMask.empty();
	}

	bool DX11GraphicsPipelineStateController::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		const auto & shaderInputSignature = getShaderInputSignature();
		const auto & descriptorInfo = shaderInputSignature.getDescriptorInfo( pParamRefID );

		ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Resource );
		ts3DebugAssert( descriptorInfo.uResourceInfo.resourceType == EShaderInputResourceType::SRVTextureImage );

		auto updatedStagesMask = makeBitmask<EShaderStageFlags>();

		if( descriptorInfo.dShaderVisibilityMask != 0 )
		{
			auto * dx11Texture = pTexture.queryInterface<DX11Texture>();
			auto * d3d11TextureSRV = dx11Texture->mD3D11DefaultSRV.Get();

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT ) )
			{
				mD3D11DeviceContext1->VSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT ) )
			{
				mD3D11DeviceContext1->HSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT ) )
			{
				mD3D11DeviceContext1->DSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT ) )
			{
				mD3D11DeviceContext1->GSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT ) )
			{
				mD3D11DeviceContext1->PSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT );
			}
		}

		return !updatedStagesMask.empty();
	}

	bool DX11GraphicsPipelineStateController::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		const auto & shaderInputSignature = getShaderInputSignature();
		const auto & descriptorInfo = shaderInputSignature.getDescriptorInfo( pParamRefID );

		ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Sampler );

		auto updatedStagesMask = makeBitmask<EShaderStageFlags>();

		if( descriptorInfo.dShaderVisibilityMask != 0 )
		{
			auto * dx11Sampler = pSampler.queryInterface<DX11Sampler>();
			auto * d3d11SamplerState = dx11Sampler->mD3D11SamplerState.Get();

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT ) )
			{
				mD3D11DeviceContext1->VSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT ) )
			{
				mD3D11DeviceContext1->HSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT ) )
			{
				mD3D11DeviceContext1->DSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT ) )
			{
				mD3D11DeviceContext1->GSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT );
			}

			if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT ) )
			{
				mD3D11DeviceContext1->PSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				updatedStagesMask.set( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT );
			}
		}

		return !updatedStagesMask.empty();
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

	void DX11GraphicsPipelineStateController::applyIAVertexStreamState( const DX11IAVertexStreamDefinition & pVertexStreamDefinition )
	{
		const auto & indexBufferBinding = pVertexStreamDefinition.indexBufferBinding;
		const auto & vertexBufferBindings = pVertexStreamDefinition.vertexBufferBindings;

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

	void DX11GraphicsPipelineStateController::applyRenderTargetBinding( const DX11RenderTargetBindingData & pRenderTargetBindingData )
	{
		mDX11CommandList->mD3D11DeviceContext1->OMSetRenderTargets(
				gpm::RT_MAX_COLOR_ATTACHMENTS_NUM,
				pRenderTargetBindingData.d3d11ColorAttachmentRTViewArray,
				pRenderTargetBindingData.d3d11DepthStencilAttachmentDSView );
	}

} // namespace ts3::gpuapi
