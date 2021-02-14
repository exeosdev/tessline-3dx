
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX12_PRESENTATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_DX12_PRESENTATION_LAYER_H__

#include "../DX12_prerequisites.h"
#include <ts3/gpuapiDX/system/DX_presentationLayer.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( DX12ScreenPresentationLayer );

	struct DX12PresentationLayerCreateInfo : public PresentationLayerCreateInfo
	{
		Bitmask<UINT> dxgiFlags = 0;
	};

	class TS3GX_DX12_CLASS DX12ScreenPresentationLayer final : public DXScreenPresentationLayer
	{
	public:
		static constexpr uint32 sMinFrameQueueSize = 2;

		ComPtr<IDXGISwapChain3> const mDXGISwapChain3 = nullptr;
		ComPtr<ID3D12Device> const mD3D12Device = nullptr;
		ComPtr<ID3D12CommandQueue> const mD3D12PresentQueue = nullptr;

		DX12ScreenPresentationLayer( DX12GPUDevice & pDevice, ComPtr<IDXGISwapChain3> pDXGISwapChain3, uint32 pFrameQueueSize = sMinFrameQueueSize );
		virtual ~DX12ScreenPresentationLayer();

		virtual void bindRenderTarget( CommandContext * pCmdContext ) override;

		virtual void invalidateRenderTarget( CommandContext * pCmdContext ) override;

		virtual void present() override;

		static DX12ScreenPresentationLayerHandle create( DX12GPUDevice & pDX12Device, const DX12PresentationLayerCreateInfo & pCreateInfo );

	private:
		bool _initializeRTVDescriptorHeap();
		bool _initializeRTVResources();

	private:
		struct FrameResource
		{
			ComPtr<ID3D12Resource> backBufferTexture;
			D3D12_CPU_DESCRIPTOR_HANDLE backBufferTextureRTVDescriptor;
		};

		uint32 _frameQueueSize;
		uint32 _currentFrameIndex;
		std::vector<FrameResource> _frameResourceArray;
		HANDLE _frameSyncFenceEvent;
		ComPtr<ID3D12Fence> _frameSyncFence;
		UINT64 _frameSyncFenceValue = 0;
		ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap;
		UINT _rtvDescriptorSize;
	};

}

#endif // __TS3DRIVER_GPUAPI_DX12_PRESENTATION_LAYER_H__
