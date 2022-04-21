
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_PRESENTATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_DX11_PRESENTATION_LAYER_H__

#include "DX11_prerequisites.h"
#include <ts3/gpuapiDX/DX_presentationLayer.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( DX11ScreenPresentationLayer );

	struct DX11PresentationLayerCreateInfo : public PresentationLayerCreateInfo
	{
		Bitmask<UINT> dxgiFlags = 0;
	};

	class TS3GX_DX11_CLASS DX11ScreenPresentationLayer final : public DXScreenPresentationLayer
	{
	public:
		ComPtr<ID3D11Device1> const mD3D11Device1 = nullptr;

		DX11ScreenPresentationLayer( DX11GPUDevice & pDevice,
                                     system::WindowHandle pSysWindow,
                                     ComPtr<IDXGISwapChain1> pDXGISwapChain );
		virtual ~DX11ScreenPresentationLayer();

		virtual void bindRenderTarget( CommandContext * pCmdContext ) override;

		virtual void invalidateRenderTarget( CommandContext * pCmdContext ) override;

		virtual void present() override;

		static DX11ScreenPresentationLayerHandle create( DX11GPUDevice & pDX11Device, const DX11PresentationLayerCreateInfo & pCreateInfo );

	private:
		ID3D11RenderTargetView * getBackBufferRTView();
		ID3D11DepthStencilView * getBackBufferDSView();
		bool _createBackBufferResources();

	private:
		ComPtr<ID3D11RenderTargetView> _backBufferRTView;
		ComPtr<ID3D11DepthStencilView> _backBufferDSView;
		ComPtr<ID3D11Texture2D> _backBufferDSTexture;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_PRESENTATION_LAYER_H__
