
#include "DX12_gpuCmdContext.h"
#include "DX12_gpuCmdManager.h"
#include "DX12_gpuDevice.h"
#include "DX12_coreAPIProxy.h"

namespace ts3::gpuapi
{

	DX12CommandContext::DX12CommandContext( DX12GPUCmdManager & pDX12GPUCmdManager,
	                                      ComPtr<ID3D12CommandAllocator> pD3D12CommandAllocator,
	                                      ComPtr<ID3D12GraphicsCommandList> pD3D12GraphicsCommandList )
	: CommandContext( pDX12GPUCmdManager )
	, mD3D12CommandAllocator( std::move( pD3D12CommandAllocator ) )
	, mD3D12GraphicsCommandList( std::move( pD3D12GraphicsCommandList ) )
	, mD3D12CommandListType( mD3D12GraphicsCommandList->GetType() )
	{ }

	DX12CommandContext::~DX12CommandContext() = default;

	void DX12CommandContext::beginCommandSequence( const CommandContextCommandSequenceBeginInfo & pInfo )
	{
		ts3DX12CmdContextVerifyGraphics( this );
		mD3D12CommandAllocator->Reset();
		mD3D12GraphicsCommandList->Reset( mD3D12CommandAllocator.Get(), nullptr );
	}

	void DX12CommandContext::endCommandSequence( const CommandContextCommandSequenceEndInfo & pInfo )
	{
		ts3DX12CmdContextVerifyGraphics( this );
		mD3D12GraphicsCommandList->Close();
	}

	void DX12CommandContext::executeSecondaryContext( CommandContext & pSecondaryCmdContext )
	{
		auto * dx12CmdContext = pSecondaryCmdContext.queryInterface<DX12CommandContext>();
		mD3D12GraphicsCommandList->ExecuteBundle( dx12CmdContext->mD3D12GraphicsCommandList.Get() );
	}

	void DX12CommandContext::clearColorTarget( const math::RGBAColorR32Norm & pColor )
	{
		ts3DX12CmdContextVerifyGraphics( this );

		const FLOAT clearColor[] = { pColor.fpRed, pColor.fpGreen, pColor.fpBlue, pColor.fpAlpha };
		for( uint32 rtIndex = 0; rtIndex < _renderTargetState.rtvDescriptorsNum; ++rtIndex )
		{
			auto & rtvDescriptorHandle = _renderTargetState.rtvDescriptorArray[rtIndex];
			mD3D12GraphicsCommandList->ClearRenderTargetView( rtvDescriptorHandle, clearColor, 0, nullptr );
		}
	}

	void DX12CommandContext::setRenderTargetState( const DX12RenderTargetState & pRenderTargetState )
	{
		memCopyUnchecked(
				&_renderTargetState,
				sizeof( DX12RenderTargetState ),
				&pRenderTargetState,
				sizeof( DX12RenderTargetState ) );

		const D3D12_CPU_DESCRIPTOR_HANDLE * rtvDescriptorArray = nullptr;
		const D3D12_CPU_DESCRIPTOR_HANDLE * dsvDescriptor = nullptr;

		if( _renderTargetState.rtvDescriptorsNum > 0 )
		{
			rtvDescriptorArray = _renderTargetState.rtvDescriptorArray.data();
		}
		if( !DX12CoreAPIProxy::checkDescriptorEmpty( _renderTargetState.dsvDescriptor ) )
		{
			dsvDescriptor = &( _renderTargetState.dsvDescriptor );
		}

		mD3D12GraphicsCommandList->OMSetRenderTargets( _renderTargetState.rtvDescriptorsNum, rtvDescriptorArray, FALSE, dsvDescriptor );
	}

}
