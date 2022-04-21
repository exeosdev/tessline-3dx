
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_SWAP_CHAIN_H__
#define __TS3DRIVER_GPUAPI_DXCOMMON_SWAP_CHAIN_H__

#include "DX_prerequisites.h"
#include <ts3/gpuapi/presentationLayer.h>
#include <ts3/system/windowSystem.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( DXPresentationLayer );

	class TS3GX_DXCOMMON_CLASS DXPresentationLayer : public PresentationLayer
	{
	public:
		DXPresentationLayer( GPUDevice & pDevice );
		virtual ~DXPresentationLayer();
	};

	class TS3GX_DXCOMMON_CLASS DXScreenPresentationLayer : public DXPresentationLayer
	{
	public:
		system::WindowHandle const mSysWindow;
		ComPtr<IDXGISwapChain1> const mDXGISwapChain1;

		DXScreenPresentationLayer( GPUDevice & pDevice, system::WindowHandle pSysWindow, ComPtr<IDXGISwapChain1> pDXGISwapChain1 ) noexcept;
		virtual ~DXScreenPresentationLayer() noexcept;

		virtual system::EventSource * getInternalSystemEventSource() const noexcept override;

		virtual void resize( uint32 pWidth, uint32 pHeight ) override final;

		virtual void setFullscreenMode( bool pEnable ) override final;

		virtual ts3::math::Vec2u32 queryRenderTargetSize() const override;

	protected:
	    static system::WindowHandle createSysWindow( DXGPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_SWAP_CHAIN_H__
