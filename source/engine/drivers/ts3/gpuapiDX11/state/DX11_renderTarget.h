
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_H__
#define __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_H__

#include "../resources/DX11_texture.h"
#include <ts3/gpuapi/resources/textureReference.h>
#include <ts3/gpuapi/state/renderTargetImmutableStates.h>

namespace ts3::gpuapi
{

	class DX11Texture;

	struct DX11RenderTargetColorAttachment
	{
		ID3D11Resource * d3d11Resource = nullptr;
		UINT d3d11SubResourceIndex;
		ComPtr<ID3D11RenderTargetView> d3d11RTView;

		explicit operator bool() const noexcept
		{
			return d3d11Resource != nullptr;
		}
	};

	struct DX11RenderTargetDepthStencilAttachment
	{
		ID3D11Resource * d3d11Resource = nullptr;
		UINT d3d11SubResourceIndex;
		ComPtr<ID3D11DepthStencilView> d3d11DSView;

		explicit operator bool() const noexcept
		{
			return d3d11Resource != nullptr;
		}
	};

	struct DX11RenderTargetResolveAttachment
	{
		ID3D11Resource * d3d11Resource = nullptr;
		UINT d3d11SubResourceIndex;
		DXGI_FORMAT targetDXGIFormat;

		explicit operator bool() const noexcept
		{
			return d3d11Resource != nullptr;
		}
	};

	struct DX11RenderTargetBindingData
	{
		Bitmask<ERTAttachmentFlags> activeAttachmentsMask = 0;

		DX11RenderTargetColorAttachment colorAttachments[gpm::RT_MAX_COLOR_ATTACHMENTS_NUM];
		DX11RenderTargetDepthStencilAttachment depthStencilAttachment;
		DX11RenderTargetResolveAttachment resolveAttachments[gpm::RT_MAX_COMBINED_ATTACHMENTS_NUM];

		ID3D11RenderTargetView * d3d11ColorAttachmentRTViewArray[gpm::RT_MAX_COLOR_ATTACHMENTS_NUM];
		ID3D11DepthStencilView * d3d11DepthStencilAttachmentDSView;
	};

	class DX11RenderTargetBindingImmutableState : public RenderTargetBindingImmutableState
	{
	public:
		DX11RenderTargetBindingData const mDX11RTBindingData;

	public:
		DX11RenderTargetBindingImmutableState(
				DX11GPUDevice & pGPUDevice,
				const RenderTargetLayout & pRenderTargetLayout,
				DX11RenderTargetBindingData pDX11RTBindingData );

		virtual ~DX11RenderTargetBindingImmutableState();

		static GpaHandle<DX11RenderTargetBindingImmutableState> createInstance(
				DX11GPUDevice & pGPUDevice,
				const RenderTargetBindingDefinition & pBindingDefinition );

		static GpaHandle<DX11RenderTargetBindingImmutableState> createForScreen(
				DX11GPUDevice & pGPUDevice,
				ComPtr<ID3D11Texture2D> pColorBuffer,
				ComPtr<ID3D11Texture2D> pDepthStencilBuffer );
	};


	namespace smutil
	{

		TS3_ATTR_NO_DISCARD RenderTargetLayout getRenderTargetLayoutForScreenDX11(
				ID3D11Texture2D * pColorBuffer,
				ID3D11Texture2D * pDepthStencilBuffer );

		TS3_ATTR_NO_DISCARD DX11RenderTargetColorAttachment createRenderTargetColorAttachmentDX11(
				DX11GPUDevice & pGPUDevice,
				const TextureReference & pAttachmentTextureRef );

		TS3_ATTR_NO_DISCARD DX11RenderTargetDepthStencilAttachment createRenderTargetDepthStencilAttachmentDX11(
				DX11GPUDevice & pGPUDevice,
				const TextureReference & pAttachmentTextureRef );

		TS3_ATTR_NO_DISCARD DX11RenderTargetResolveAttachment createRenderTargetResolveAttachmentDX11(
				const TextureReference & pAttachmentTextureRef );

		TS3_ATTR_NO_DISCARD DX11RenderTargetBindingData createRenderTargetBindingDataDX11(
				DX11GPUDevice & pGPUDevice,
				const RenderTargetBindingDefinition & pBindingDefinition );

		void renderPassClearRenderTargetDX11(
				ID3D11DeviceContext1 * pD3D1DeviceContext,
				const DX11RenderTargetBindingData & pRenderTargetBinding,
				const RenderPassConfiguration & pRenderPassConfiguration,
				const GraphicsPipelineDynamicState & pDynamicState );

		void renderPassResolveRenderTargetDX11(
				ID3D11DeviceContext1 * pD3D1DeviceContext,
				const DX11RenderTargetBindingData & pRenderTargetBinding,
				const RenderPassConfiguration & pRenderPassConfiguration,
				const GraphicsPipelineDynamicState & pDynamicState );

	}
	
} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_H__
