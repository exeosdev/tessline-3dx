
#include "DX12_coreAPIProxy.h"
#include <ts3/system/windowSystem.h>

namespace ts3::gpuapi
{

	ComPtr<ID3D12Debug> DX12CoreAPIProxy::initializeD3D12DebugInterface( Bitmask<GPUDriverConfigFlags> pDriverConfigFlags )
	{
		if( !pDriverConfigFlags.isSet( GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT ) )
		{
			return nullptr;
		}

		ComPtr<ID3D12Debug> d3d12DebugInterface;
		auto hResult = D3D12GetDebugInterface( IID_PPV_ARGS( &d3d12DebugInterface ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		d3d12DebugInterface->EnableDebugLayer();

		return d3d12DebugInterface;
	}

	ComPtr<ID3D12Device> DX12CoreAPIProxy::createD3D12Device()
	{

		ComPtr<ID3D12Device> d3d12Device;
		auto hResult = D3D12CreateDevice( nullptr,
		                                  D3D_FEATURE_LEVEL_12_1,
		                                  IID_PPV_ARGS( &d3d12Device ) );

		if( FAILED( hResult ) )
		{
			hResult = D3D12CreateDevice( nullptr,
			                             D3D_FEATURE_LEVEL_12_0,
			                             IID_PPV_ARGS( &d3d12Device ) );
			if( FAILED( hResult ) )
			{
				return nullptr;
			}
		}

		return d3d12Device;
	}

	ComPtr<IDXGIFactory3> DX12CoreAPIProxy::createDXGIFactoryForD3D12Device( const ComPtr<ID3D12Device> & pD3D12Device,
	                                                                 const ComPtr<ID3D12Debug> & pD3D12DebugInterface )
	{
		UINT dxgiFactoryCreateFlags = 0;
		if( pD3D12DebugInterface )
		{
			dxgiFactoryCreateFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;
		auto hResult = ::CreateDXGIFactory2( dxgiFactoryCreateFlags, IID_PPV_ARGS( &dxgiFactory2 ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		ComPtr<IDXGIFactory3> dxgiFactory3 = nullptr;
		hResult = dxgiFactory2->QueryInterface( IID_PPV_ARGS( &dxgiFactory3 ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return dxgiFactory3;
	}

	ComPtr<ID3D12CommandQueue> DX12CoreAPIProxy::createD3D12CommandQueue( const ComPtr<ID3D12Device> & pD3D12Device,
	                                                              D3D12_COMMAND_LIST_TYPE pD3D12CommandListType )
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDesc.NodeMask = 0;
		commandQueueDesc.Type = pD3D12CommandListType;

		ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
		auto hResult = pD3D12Device->CreateCommandQueue( &commandQueueDesc, IID_PPV_ARGS( &commandQueue ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return commandQueue;
	}

	ComPtr<IDXGISwapChain3> DX12CoreAPIProxy::createD3D12SwapChainForExfWindow( const ComPtr<ID3D12CommandQueue> & pD3D12PresentCommandQueue,
	                                                                    void * pExfSysWindow, /* It must be an system::Window */
	                                                                    Bitmask<UINT> pDXGIFlags )
	{
		UINT dxgiFactoryCreateFlags = 0;
		if( pDXGIFlags.isSet( DXGI_CREATE_FACTORY_DEBUG ) )
		{
			dxgiFactoryCreateFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;
		auto hResult = ::CreateDXGIFactory2( dxgiFactoryCreateFlags, IID_PPV_ARGS( &dxgiFactory2 ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		auto * exfWindowPtr = static_cast<system::Window *>( pExfSysWindow );
		auto windowHWND = reinterpret_cast<HWND>( exfWindowPtr->getNativeHandle() );
		auto presentationLayerSize = exfWindowPtr->getClientSize();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
		ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC1 ) );
		swapChainDesc.Width = presentationLayerSize.x;
		swapChainDesc.Height = presentationLayerSize.y;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 0u;

		ComPtr<IDXGISwapChain1> dxgiSwapChain = nullptr;
		hResult = dxgiFactory2->CreateSwapChainForHwnd( pD3D12PresentCommandQueue.Get(),
		                                                windowHWND,
		                                                &swapChainDesc,
		                                                nullptr,
		                                                nullptr,
		                                                dxgiSwapChain.GetAddressOf() );

		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		ComPtr<IDXGISwapChain3> dxgiSwapChain3 = nullptr;
		hResult = dxgiSwapChain->QueryInterface( IID_PPV_ARGS( &dxgiSwapChain3 ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return dxgiSwapChain3;
	}

	D3D12CommandListData DX12CoreAPIProxy::createD3D12CommandList( const ComPtr<ID3D12Device> & pD3D12Device,
	                                                       D3D12_COMMAND_LIST_TYPE pD3D12CommandListType )
	{
		ComPtr<ID3D12CommandAllocator> d3d12CommandAllocator;
		auto hResult = pD3D12Device->CreateCommandAllocator( pD3D12CommandListType, IID_PPV_ARGS( &d3d12CommandAllocator ) );
		if( FAILED( hResult ) )
		{
			return {};
		}

		ComPtr<ID3D12GraphicsCommandList> d3d12GraphicsCommandList;
		hResult = pD3D12Device->CreateCommandList( 0,
		                                           pD3D12CommandListType,
		                                           d3d12CommandAllocator.Get(),
		                                           nullptr,
		                                           IID_PPV_ARGS( &d3d12GraphicsCommandList ) );
		if( FAILED( hResult ) )
		{
			return {};
		}

		D3D12CommandListData d3d12CommandListData;
		d3d12CommandListData.commandAllocator = std::move( d3d12CommandAllocator );
		d3d12CommandListData.graphicsCommandList = std::move( d3d12GraphicsCommandList );
		d3d12CommandListData.graphicsCommandList->Close();

		return d3d12CommandListData;
	}

	D3D12_COMMAND_LIST_TYPE DX12CoreAPIProxy::translateCommandListType( ECommandContextType pContextType,
	                                                            Bitmask<GPUCmdCommandClassFlags> pCommandClassFlags )
	{
		auto commandListType = cvD3D12CommandListTypeInvalid;
		if( pContextType == ECommandContextType::Secondary )
		{
			commandListType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
		}
		else if( pContextType == ECommandContextType::Direct )
		{
			if( pCommandClassFlags.isSet( GPU_CMD_COMMAND_CLASS_GRAPHICS_BIT ) )
			{
				commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
			}
			else if( pCommandClassFlags.isSet( GPU_CMD_COMMAND_CLASS_COMPUTE_BIT ) )
			{
				commandListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			}
			else if( pCommandClassFlags.isSet( GPU_CMD_COMMAND_CLASS_TRANSFER_BIT ) )
			{
				commandListType = D3D12_COMMAND_LIST_TYPE_COPY;
			}
		}
		return commandListType;
	}

}
