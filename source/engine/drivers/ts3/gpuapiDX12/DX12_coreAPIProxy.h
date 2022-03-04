
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX12_CORE_LAYER_H__
#define __TS3DRIVER_GPUAPI_DX12_CORE_LAYER_H__

#include "DX12_prerequisites.h"
#include <ts3/gpuapiDX/DX_coreAPIProxy.h>
#include <ts3/gpuapi/cmds/commonCommandDefs.h>

namespace ts3
{
namespace gpuapi
{

	struct D3D12CommandListData
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;

		explicit operator bool() const
		{
			return commandAllocator && graphicsCommandList;
		}
	};

	namespace DX12CoreAPIProxy
	{

		using namespace DXCoreAPIProxy;

		inline constexpr bool checkDescriptorEmpty( const D3D12_CPU_DESCRIPTOR_HANDLE & pDescriptor )
		{
			return pDescriptor.ptr == cvD3D12CPUDescriptorPtrInvalid;
		}

		ComPtr<ID3D12Debug> initializeD3D12DebugInterface( Bitmask<GPUDriverConfigFlags> pDriverConfigFlags );

		ComPtr<ID3D12Device> createD3D12Device();

		ComPtr<IDXGIFactory3> createDXGIFactoryForD3D12Device( const ComPtr<ID3D12Device> & pD3D12Device,
		                                                       const ComPtr<ID3D12Debug> & pD3D12DebugInterface );

		ComPtr<ID3D12CommandQueue> createD3D12CommandQueue( const ComPtr<ID3D12Device> & pD3D12Device,
		                                                    D3D12_COMMAND_LIST_TYPE pD3D12CommandListType );

		ComPtr<IDXGISwapChain3> createD3D12SwapChainForExfWindow( const ComPtr<ID3D12CommandQueue> & pD3D12PresentCommandQueue,
		                                                          void * pExfSysWindow, /* It must be an system::Window */
		                                                          Bitmask<UINT> pDXGIFlags );

		D3D12CommandListData createD3D12CommandList( const ComPtr<ID3D12Device> & pD3D12Device,
		                                             D3D12_COMMAND_LIST_TYPE pD3D12CommandListType );

		D3D12_COMMAND_LIST_TYPE translateCommandListType( ECommandContextType pContextType,
		                                                  Bitmask<GPUCmdCommandClassFlags> pCommandClassFlags );

	}

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DX12_CORE_LAYER_H__
