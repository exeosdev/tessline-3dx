
#include "DX_presentationLayer.h"
#include "DX_gpuDevice.h"
#include <ts3/system/displayManager.h>
#include <ts3/system/windowSystem.h>

namespace ts3::gpuapi
{

	DXPresentationLayer::DXPresentationLayer( GPUDevice & pDevice )
	: PresentationLayer( pDevice )
	{}

	DXPresentationLayer::~DXPresentationLayer() = default;


	DXScreenPresentationLayer::DXScreenPresentationLayer( GPUDevice & pDevice, SysWindowHandle pSysWindow, ComPtr<IDXGISwapChain1> pDXGISwapChain1 ) noexcept
	: DXPresentationLayer( pDevice )
	, mSysWindow( pSysWindow )
	, mDXGISwapChain1( std::move( pDXGISwapChain1 ) )
	{}

	DXScreenPresentationLayer::~DXScreenPresentationLayer() noexcept = default;

	SysEventSource * DXScreenPresentationLayer::querySysEventSourceObject() const noexcept
	{
		return mSysWindow.get();
	}

	void DXScreenPresentationLayer::resize( uint32 pWidth, uint32 pHeight )
	{
	}

	void DXScreenPresentationLayer::setFullscreenMode( bool pEnable )
	{
	}

	ts3::math::Vec2u32 DXScreenPresentationLayer::queryRenderTargetSize() const
	{
		return mSysWindow->getClientAreaSize();
	}

	SysWindowHandle DXScreenPresentationLayer::createSysWindow( DXGPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo )
	{
		try
		{
			auto sysContext = pDevice.mSysContext;
			auto sysWindowManager = pCreateInfo.sysWindowManager;

			if( !sysWindowManager )
			{
				auto sysDisplayManager = pCreateInfo.sysDisplayManager;
				if( !sysDisplayManager )
				{
					sysDisplayManager = SysDisplayManager::create( sysContext );
				}

				sysWindowManager = SysWindowManager::create( sysDisplayManager );
			}

			ts3::SysWindowCreateInfo windowCreateInfo;
			windowCreateInfo.properties.title = "DXWindow";

			if( pCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT ) )
			{
				windowCreateInfo.properties.geometry.size = ts3::cvSysWindowSizeMax;
				windowCreateInfo.properties.geometry.frameStyle = ts3::SysWindowFrameStyle::Overlay;
			}
			else
			{
				windowCreateInfo.properties.geometry.position = pCreateInfo.screenRect.offset;
				windowCreateInfo.properties.geometry.size = pCreateInfo.screenRect.size;
				windowCreateInfo.properties.geometry.frameStyle = ts3::SysWindowFrameStyle::Fixed;
			}

			auto sysWindow = sysWindowManager->createWindow( windowCreateInfo );

			if( pCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT ) )
			{
				::SetCapture( sysWindow->mNativeData.hwnd );
				::ShowCursor( FALSE );
			}

			return sysWindow;
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return nullptr;
	}

}
