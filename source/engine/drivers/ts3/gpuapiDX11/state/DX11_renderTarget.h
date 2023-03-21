
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_H__
#define __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/state/renderTargetImmutableStates.h>

namespace ts3::gpuapi
{

	struct DX11RenderTargetResourceState
	{
		ComPtr<ID3D11View> colorAttachmentViewRef[gpm::RT_MAX_COLOR_ATTACHMENTS_NUM];
		ID3D11RenderTargetView * colorAttachmentRTVArray[gpm::RT_MAX_COLOR_ATTACHMENTS_NUM];
		ComPtr<ID3D11View> depthStencilAttachmentViewRef;
		ID3D11DepthStencilView * depthStencilAttachmentDSV;
	};

	class DX11RenderTargetBindingImmutableState : public RenderTargetBindingImmutableState
	{
	public:
		DX11RenderTargetResourceState const mDX11RTResourceState;

	public:
		DX11RenderTargetBindingImmutableState(
				DX11GPUDevice & pGPUDevice,
				const RenderTargetLayout & pRenderTargetLayout,
				DX11RenderTargetResourceState pDX11RTResourceState );

		virtual ~DX11RenderTargetBindingImmutableState();

		static GpaHandle<DX11RenderTargetBindingImmutableState> createInstance(
				DX11GPUDevice & pGPUDevice,
				const RenderTargetBindingDefinition & pBindingDefinition );

		static GpaHandle<DX11RenderTargetBindingImmutableState> createForScreen(
				DX11GPUDevice & pGPUDevice,
				ComPtr<IDXGISwapChain1> pDXGISwapChain );
	};


	namespace smutil
	{

		ComPtr<ID3D11RenderTargetView> createTextureColorAttachmentView(
				DX11Texture & pDX11Texture,
				const RenderTargetAttachmentBinding & pAttachmentBinding );

		ComPtr<ID3D11DepthStencilView> createTextureDepthStencilAttachmentView(
				DX11Texture & pDX11Texture,
				const RenderTargetAttachmentBinding & pAttachmentBinding );

	}
	
} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_H__
