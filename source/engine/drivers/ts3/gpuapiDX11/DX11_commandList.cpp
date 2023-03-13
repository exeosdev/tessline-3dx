
#include "DX11_commandList.h"
#include "DX11_commandSystem.h"
#include "DX11_coreAPIProxy.h"
#include "DX11_gpuDevice.h"
#include "resources/DX11_gpuBuffer.h"
#include "resources/DX11_sampler.h"
#include "resources/DX11_shader.h"
#include "resources/DX11_texture.h"
#include "state/DX11_pipelineStateObject.h"
#include "state/DX11_renderTargetStateObject.h"
#include "state/DX11_vertexStreamStateObject.h"
#include <ts3/gpuapi/commandContext.h>
#include <ts3/gpuapi/resources/gpuBuffer.h>
#include <ts3/gpuapi/resources/shader.h>
#include <ts3/gpuapi/resources/texture.h>

namespace ts3::gpuapi
{

	DX11CommandList::DX11CommandList( DX11CommandSystem & pDX11CommandSystem, ECommandListType pListType, ComPtr<ID3D11DeviceContext1> pD3D11DeviceContext1 )
	: CommandList( pDX11CommandSystem, pListType )
	, mD3D11Device1( pDX11CommandSystem.mD3D11Device1 )
	, mD3D11DeviceContext1( std::move( pD3D11DeviceContext1 ) )
	, _stateController( *this, mGPUDevice.queryInterface<DX11GPUDevice>()->getDescriptorCache() )
	{ }

	DX11CommandList::~DX11CommandList() = default;

	void DX11CommandList::beginCommandSequence()
	{
		CommandList::beginCommandSequence();

		if( _d3d11ExecutionSyncQuery )
		{
			mD3D11DeviceContext1->Begin( _d3d11ExecutionSyncQuery.Get() );
		}
	}

	void DX11CommandList::endCommandSequence()
	{
		if( _d3d11ExecutionSyncQuery )
		{
			mD3D11DeviceContext1->End( _d3d11ExecutionSyncQuery.Get() );
		}

		// _stateController.resetInternalState();

		CommandList::endCommandSequence();
	}

	void DX11CommandList::executeDeferredContext( CommandContextDeferred & pDeferredContext )
	{
		auto * dx11CommandList = pDeferredContext.mCommandList->queryInterface<DX11CommandList>();

		ComPtr<ID3D11CommandList> d3d11RecorderCommandList;
		dx11CommandList->mD3D11DeviceContext1->FinishCommandList( FALSE, &d3d11RecorderCommandList );

		mD3D11DeviceContext1->ExecuteCommandList( d3d11RecorderCommandList.Get(), FALSE );
	}

	bool DX11CommandList::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		return _stateController.setGraphicsPipelineStateObject( pGraphicsPipelineSO );
	}

	bool DX11CommandList::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		return _stateController.setVertexStreamStateObject( pVertexStreamSO );
	}

	bool DX11CommandList::setRenderTargetStateObject( const RenderTargetStateObject & pRenderTargetSO )
	{
		const auto & commonConfig = _stateController.getCommonConfig();
		const auto & renderTargetLayout = commonConfig.soGraphicsPipeline->mRenderTargetLayout;

		auto * dx11RenderTargetSO = pRenderTargetSO.queryInterface<DX11RenderTargetStateObject>();
		if( !checkRenderTargetLayoutCompatibility( dx11RenderTargetSO->mRTResourceBinding, renderTargetLayout ) )
		{
			ts3DebugInterrupt();
			return false;
		}

		mD3D11DeviceContext1->OMSetRenderTargets( E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM,
		                                          dx11RenderTargetSO->mDX11RTResourceState.colorAttachmentRTVArray,
		                                          dx11RenderTargetSO->mDX11RTResourceState.depthStencilAttachmentDSV );

		return true;
	}

	void DX11CommandList::clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask )
	{
		if( pAttachmentMask == 0 )
		{
			return;
		}

		ComPtr<ID3D11RenderTargetView> renderTargetViewArray[E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		ComPtr<ID3D11DepthStencilView> depthStencilView;

		mD3D11DeviceContext1->OMGetRenderTargets( E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM,
		                                          &( renderTargetViewArray[0] ),
		                                          depthStencilView.GetAddressOf() );

		const auto & renderTargetClearConfig = getRenderTargetClearConfig();

		if( pAttachmentMask.isSetAnyOf( E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_BIT | E_RENDER_TARGET_ATTACHMENT_FLAG_STENCIL_BIT ) )
		{
			ts3DebugAssert( depthStencilView );
			auto dx11DSClearFlags = DX11CoreAPIProxy::translateDX11ClearDSFlags( pAttachmentMask );
			mD3D11DeviceContext1->ClearDepthStencilView( depthStencilView.Get(),
			                                             dx11DSClearFlags,
			                                             static_cast<FLOAT>( renderTargetClearConfig.depthClearValue ),
			                                             static_cast<UINT8>( renderTargetClearConfig.stencilClearValue ) );

			pAttachmentMask.unset( E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_BIT | E_RENDER_TARGET_ATTACHMENT_FLAG_STENCIL_BIT );
		}

		for( uint32 colorBufferIndex = 0; ( colorBufferIndex < E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM ) && ( pAttachmentMask != 0 ); ++colorBufferIndex )
		{
			auto colorBufferMask = E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_0_BIT << colorBufferIndex;
			if( pAttachmentMask.isSet( colorBufferMask ) )
			{
				const auto & renderTargetView = staticArrayElement( renderTargetViewArray, colorBufferIndex );
				ts3DebugAssert( renderTargetView );

				mD3D11DeviceContext1->ClearRenderTargetView( renderTargetView.Get(), &( renderTargetClearConfig.colorClearValue.rgbaArray[0] ) );

				pAttachmentMask.unset( colorBufferMask );
			}
		}

		ts3DebugAssert( pAttachmentMask == 0 );
	}

	void DX11CommandList::setViewport( const ViewportDesc & pViewportDesc )
	{
		D3D11_VIEWPORT viewportDesc;
		viewportDesc.TopLeftX = static_cast<float>( pViewportDesc.vpRect.opX );
		viewportDesc.TopLeftY = static_cast<float>( pViewportDesc.vpRect.opY );
		viewportDesc.Width = static_cast<float>( pViewportDesc.vpRect.sizeX );
		viewportDesc.Height = static_cast<float>( pViewportDesc.vpRect.sizeY );
		viewportDesc.MinDepth = pViewportDesc.depthRange.zNear;
		viewportDesc.MaxDepth = pViewportDesc.depthRange.zFar;

		mD3D11DeviceContext1->RSSetViewports( 1, &viewportDesc );
	}

	bool DX11CommandList::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		return true;
	}

	bool DX11CommandList::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		const auto & commonConfig = _stateController.getCommonConfig();
		if( const auto & inputSignature = commonConfig.soGraphicsPipeline->mShaderInputSignature )
		{
			const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
			ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Resource );
			ts3DebugAssert( descriptorInfo.uResourceInfo.resourceType == EShaderInputResourceType::CBVConstantBuffer );

			if( descriptorInfo.dShaderVisibilityMask != 0 )
			{
				auto * dx11Buffer = pConstantBuffer.queryInterface<DX11GPUBuffer>();
				auto * d3d11Buffer = dx11Buffer->mD3D11Buffer.Get();

				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT ) )
				{
					mD3D11DeviceContext1->VSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_TESS_CONTROL_BIT ) )
				{
					mD3D11DeviceContext1->HSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_TESS_EVALUATION_BIT ) )
				{
					mD3D11DeviceContext1->DSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT ) )
				{
					mD3D11DeviceContext1->GSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT ) )
				{
					mD3D11DeviceContext1->PSSetConstantBuffers( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11Buffer );
				}

				return true;
			}

		}
		return true;
	}

	bool DX11CommandList::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		const auto & commonConfig = _stateController.getCommonConfig();
		if( const auto & inputSignature = commonConfig.soGraphicsPipeline->mShaderInputSignature )
		{
			const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
			ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Resource );
			ts3DebugAssert( descriptorInfo.uResourceInfo.resourceType == EShaderInputResourceType::SRVTextureImage );

			if( descriptorInfo.dShaderVisibilityMask != 0 )
			{
				auto * dx11Texture = pTexture.queryInterface<DX11Texture>();
				auto * d3d11TextureSRV = dx11Texture->mD3D11DefaultSRV.Get();

				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT ) )
				{
					mD3D11DeviceContext1->VSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_TESS_CONTROL_BIT ) )
				{
					mD3D11DeviceContext1->HSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_TESS_EVALUATION_BIT ) )
				{
					mD3D11DeviceContext1->DSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT ) )
				{
					mD3D11DeviceContext1->GSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT ) )
				{
					mD3D11DeviceContext1->PSSetShaderResources( descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, 1, &d3d11TextureSRV );
				}

				return true;
			}

		}
		return true;
	}

	bool DX11CommandList::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		const auto & commonConfig = _stateController.getCommonConfig();
		if( const auto & inputSignature = commonConfig.soGraphicsPipeline->mShaderInputSignature )
		{
			const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
			ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Sampler );

			if( descriptorInfo.dShaderVisibilityMask != 0 )
			{
				auto * dx11Sampler = pSampler.queryInterface<DX11Sampler>();
				auto * d3d11SamplerState = dx11Sampler->mD3D11SamplerState.Get();
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT ) )
				{
					mD3D11DeviceContext1->VSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_TESS_CONTROL_BIT ) )
				{
					mD3D11DeviceContext1->HSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_TESS_EVALUATION_BIT ) )
				{
					mD3D11DeviceContext1->DSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT ) )
				{
					mD3D11DeviceContext1->GSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				}
				if( descriptorInfo.dShaderVisibilityMask.isSet( E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT ) )
				{
					mD3D11DeviceContext1->PSSetSamplers( descriptorInfo.uSamplerInfo.samplerBindingIndex, 1, &d3d11SamplerState );
				}

				return true;
			}

		}
		return true;
	}

	void DX11CommandList::drawDirectIndexed( native_uint pIndicesNum, native_uint pIndicesOffset )
	{
		updatePipelineState();

		mD3D11DeviceContext1->DrawIndexed( pIndicesNum, pIndicesOffset, 0 );
	}

	void DX11CommandList::drawDirectIndexedInstanced( native_uint pIndicesNumPerInstance, native_uint pInstancesNum, native_uint pIndicesOffset )
	{
		updatePipelineState();

		mD3D11DeviceContext1->DrawIndexedInstanced( pIndicesNumPerInstance, pInstancesNum, pIndicesOffset, 0, 0 );
	}

	void DX11CommandList::drawDirectNonIndexed( native_uint pVerticesNum, native_uint pVerticesOffset )
	{
		updatePipelineState();

		mD3D11DeviceContext1->Draw( pVerticesNum, pVerticesOffset );
	}

	void DX11CommandList::drawDirectNonIndexedInstanced( native_uint pVerticesNumPerInstance, native_uint pInstancesNum, native_uint pVerticesOffset )
	{
	}

	ID3D11Query * DX11CommandList::releaseExecutionSyncQuery()
	{
		auto * syncQuery = _d3d11ExecutionSyncQuery.Get();
		_d3d11ExecutionSyncQuery.Reset();
		return syncQuery;
	}

	void DX11CommandList::updatePipelineState()
	{
		_stateController.updatePipelineState();
	}

	ComPtr<ID3D11Query> DX11CommandList::_createExecutionSyncQuery( ComPtr<ID3D11Device1> pD3D11Device )
	{
		D3D11_QUERY_DESC syncQueryDesc;
		syncQueryDesc.Query = D3D11_QUERY_EVENT;
		syncQueryDesc.MiscFlags = 0;

		ComPtr<ID3D11Query> d3d11Query;
		auto hResult = pD3D11Device->CreateQuery( &syncQueryDesc, d3d11Query.GetAddressOf() );
		if( FAILED( hResult ) || !d3d11Query )
		{
			ts3DebugInterrupt();
		}

		return d3d11Query;
	}

} // namespace ts3::gpuapi
