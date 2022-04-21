
#include "DX12_presentationLayer.h"
#include "../DX12_gpuDevice.h"
#include "../DX12_gpuCmdContext.h"
#include "../DX12_gpuCmdManager.h"
#include "../DX12_coreAPIProxy.h"

namespace ts3::gpuapi
{

	DX12ScreenPresentationLayer::DX12ScreenPresentationLayer( DX12GPUDevice & pDevice, ComPtr<IDXGISwapChain3> pDXGISwapChain3, uint32 pFrameQueueSize )
	: DXScreenPresentationLayer( pDevice, pDXGISwapChain3 )
	, mDXGISwapChain3( std::move( pDXGISwapChain3 ) )
	, mD3D12Device( pDevice.mD3D12Device )
	, mD3D12PresentQueue( pDevice.getD3D12DeviceQueue( E_DEVICE_COMMAND_QUEUE_ID_PRESENT ) )
	, _frameQueueSize( getMaxOf( pFrameQueueSize, sMinFrameQueueSize ) )
	, _currentFrameIndex( 0 )
	, _rtvDescriptorSize( 0 )
	, _frameSyncFenceEvent( nullptr )
	, _frameSyncFenceValue( 0 )
	{}

	DX12ScreenPresentationLayer::~DX12ScreenPresentationLayer()
	{
		if( _frameSyncFenceEvent != nullptr )
		{
			::CloseHandle( _frameSyncFenceEvent );
			_frameSyncFenceEvent = nullptr;
		}
	}

	DX12ScreenPresentationLayerHandle DX12ScreenPresentationLayer::create( DX12GPUDevice & pDX12Device, const DX12PresentationLayerCreateInfo & pCreateInfo )
	{
		auto * d3d12PresentQueue = pDX12Device.getD3D12DeviceQueue( E_DEVICE_COMMAND_QUEUE_ID_PRESENT );
		ts3DebugAssert( d3d12PresentQueue );

		auto * exfWindow = createSysWindow( pDX12Device, pCreateInfo );
		ts3DebugAssert( exfWindow );

		auto dxgiSwapChain3 = DX12CoreAPIProxy::createD3D12SwapChainForExfWindow( d3d12PresentQueue, exfWindow, pCreateInfo.dxgiFlags );
		ts3DebugAssert( dxgiSwapChain3 );

		auto presentationLayer = createGPUAPIObject<DX12ScreenPresentationLayer>( pDX12Device, std::move( dxgiSwapChain3 ) );
		presentationLayer->_initializeRTVDescriptorHeap();
		presentationLayer->_initializeRTVResources();

		return presentationLayer;
	}

	void DX12ScreenPresentationLayer::bindRenderTarget( CommandContext * pCmdContext )
	{
		auto * dx12CmdContext = pCmdContext->getInterface<DX12CommandContext>();
		ts3DebugAssert( dx12CmdContext->mD3D12GraphicsCommandList );

		auto & currentFrameResource = _frameResourceArray.at( _currentFrameIndex );

		D3D12_RESOURCE_BARRIER rtStateTransition;
		rtStateTransition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		rtStateTransition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		rtStateTransition.Transition.pResource = currentFrameResource.backBufferTexture.Get();
		rtStateTransition.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		rtStateTransition.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		rtStateTransition.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		dx12CmdContext->mD3D12GraphicsCommandList->ResourceBarrier( 1, &rtStateTransition );

		DX12RenderTargetState renderTargetState;
		renderTargetState.rtvDescriptorArray[0] = currentFrameResource.backBufferTextureRTVDescriptor;
		renderTargetState.rtvDescriptorsNum = 1;
		renderTargetState.dsvDescriptor = cvD3D12CPUDescriptorEmpty;
		dx12CmdContext->setRenderTargetState( renderTargetState );
	}

	void DX12ScreenPresentationLayer::invalidateRenderTarget( CommandContext * pCmdContext )
	{
		auto * dx12CmdContext = pCmdContext->getInterface<DX12CommandContext>();
		ts3DebugAssert( dx12CmdContext->mD3D12GraphicsCommandList );

		auto & currentFrameResource = _frameResourceArray.at( _currentFrameIndex );

		D3D12_RESOURCE_BARRIER rtStateTransition;
		rtStateTransition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		rtStateTransition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		rtStateTransition.Transition.pResource = currentFrameResource.backBufferTexture.Get();
		rtStateTransition.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		rtStateTransition.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		rtStateTransition.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		dx12CmdContext->mD3D12GraphicsCommandList->ResourceBarrier( 1, &rtStateTransition );
	}

	void DX12ScreenPresentationLayer::present()
	{
	#if ( 1 )
		mDXGISwapChain3->Present( 1, 0 );

		const UINT64 frameSyncFenceValue = _frameSyncFenceValue;
		mD3D12PresentQueue->Signal( _frameSyncFence.Get(), frameSyncFenceValue );
		++_frameSyncFenceValue;

		if( _frameSyncFence->GetCompletedValue() < frameSyncFenceValue )
		{
			_frameSyncFence->SetEventOnCompletion( frameSyncFenceValue, _frameSyncFenceEvent );
			::WaitForSingleObjectEx( _frameSyncFenceEvent, INFINITE, FALSE );
		}

		_currentFrameIndex = mDXGISwapChain3->GetCurrentBackBufferIndex();
	#else
		auto * dx12CmdDeviceQueue = pCmdDeviceQueue->getAs<DX12DeviceCommandQueue>();
		ts3DebugAssert( dx12CmdDeviceQueue->mD3D12CommandQueue );

		mDXGISwapChain3->Present( 1, 0 );

		auto & currentFrameResource = _frameResourceArray[_currentFrameIndex];

		dx12CmdDeviceQueue->mD3D12CommandQueue->Signal( _frameSyncFence.Get(), currentFrameResource.frameStatusFenceValue );

		_currentFrameIndex = mDXGISwapChain3->GetCurrentBackBufferIndex();

		auto & nextFrameResource = _frameResourceArray[_currentFrameIndex];

		if( _frameSyncFence->GetCompletedValue() < nextFrameResource.frameStatusFenceValue )
		{
			_frameSyncFence->SetEventOnCompletion( nextFrameResource.frameStatusFenceValue, _frameSyncFenceEvent );
			::WaitForSingleObjectEx( _frameSyncFenceEvent, INFINITE, FALSE );
		}

		nextFrameResource.frameStatusFenceValue += 1;
	#endif
	}

	bool DX12ScreenPresentationLayer::_initializeRTVDescriptorHeap()
	{
		_frameSyncFenceEvent = ::CreateEventA( nullptr, FALSE, FALSE, nullptr );

		auto hResult = mD3D12Device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &_frameSyncFence ) );
		if( FAILED( hResult ) )
		{
			return false;
		}

		D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
		rtvDescriptorHeapDesc.NumDescriptors = _frameQueueSize;
		rtvDescriptorHeapDesc.NodeMask = 0;
		rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		hResult = mD3D12Device->CreateDescriptorHeap( &rtvDescriptorHeapDesc, IID_PPV_ARGS( &_rtvDescriptorHeap ) );
		if( FAILED( hResult ) )
		{
			return false;
		}

		_rtvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );

		return true;
	}

	bool DX12ScreenPresentationLayer::_initializeRTVResources()
	{
		ts3DebugAssert( _rtvDescriptorHeap && ( _rtvDescriptorSize > 0 ) );

		// Get the CPU handle for our previously created descriptor heap. Using the base pointer and the size of a
		// single descriptor (_rtvDescriptorSize) we can then manually compute offsets for all descriptors (see below).
		D3D12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		_frameResourceArray.resize( _frameQueueSize );

		for( uint32 frameIndex = 0; frameIndex < _frameQueueSize; ++frameIndex )
		{
			auto & frameResources = _frameResourceArray[frameIndex];

			auto hResult = mDXGISwapChain3->GetBuffer( frameIndex, IID_PPV_ARGS( &( frameResources.backBufferTexture ) ) );
			if( FAILED( hResult ) )
			{
				return false;
			}

			// Create the descriptor for ith frame. For 2D textures we can provide NULL descriptor, because all
			// required properties (dimensions, format, mips, array slices) can be obtained from the resource.
			mD3D12Device->CreateRenderTargetView( frameResources.backBufferTexture.Get(), nullptr, rtvDescriptorHandle );
			// Store the descriptor with its resource.
			frameResources.backBufferTextureRTVDescriptor = rtvDescriptorHandle;
			// Move the descriptor heap pointer to the next descriptor region.
			rtvDescriptorHandle.ptr += static_cast<ptr_value_t>( _rtvDescriptorSize );
		}

		return true;
	}
	
} // namespace ts3::gpuapi
