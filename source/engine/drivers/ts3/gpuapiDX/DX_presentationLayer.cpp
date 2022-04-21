
#include "DX_presentationLayer.h"
#include "DX_gpuDevice.h"
#include <ts3/system/displaySystem.h>
#include <ts3/system/windowSystem.h>
#include <ts3/system/windowNative.h>

namespace ts3::gpuapi
{

	DXPresentationLayer::DXPresentationLayer( GPUDevice & pDevice )
	: PresentationLayer( pDevice )
	{}

	DXPresentationLayer::~DXPresentationLayer() = default;


	DXScreenPresentationLayer::DXScreenPresentationLayer( GPUDevice & pDevice, system::WindowHandle pSysWindow, ComPtr<IDXGISwapChain1> pDXGISwapChain1 ) noexcept
	: DXPresentationLayer( pDevice )
	, mSysWindow( pSysWindow )
	, mDXGISwapChain1( std::move( pDXGISwapChain1 ) )
	{}

	DXScreenPresentationLayer::~DXScreenPresentationLayer() noexcept = default;

	system::EventSource * DXScreenPresentationLayer::getInternalSystemEventSource() const noexcept
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

	system::WindowHandle DXScreenPresentationLayer::createSysWindow( DXGPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo )
	{
		try
		{
			auto sysWindowManager = pCreateInfo.sysWindowManager;
			if( !sysWindowManager )
			{
			    auto sysDisplayManager = pCreateInfo.sysDisplayManager;
			    if( !sysDisplayManager )
			    {
			        ts3DebugAssert( pDevice.mSysContext );
			        sysDisplayManager = pDevice.mSysContext->createDisplayManager();
			    }
			    ts3DebugAssert( pDevice.mSysContext );
			    sysWindowManager = pDevice.mSysContext->createWindowManager( sysDisplayManager );
			}

			system::WindowCreateInfo windowCreateInfo;
			windowCreateInfo.title = "DXWindow";

			if( pCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT ) )
			{
				windowCreateInfo.frameGeometry.size = system::CX_FRAME_SIZE_MAX;
				windowCreateInfo.frameGeometry.style = system::EFrameStyle::Overlay;
			}
			else
			{
				windowCreateInfo.frameGeometry.position = pCreateInfo.screenRect.offset;
				windowCreateInfo.frameGeometry.size = pCreateInfo.screenRect.size;
				windowCreateInfo.frameGeometry.style = system::EFrameStyle::Fixed;
			}

			auto sysWindow = sysWindowManager->createWindow( windowCreateInfo );

        #if( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_WINDESKTOP )
			if( pCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT ) )
			{
			    auto * win32Window = sysWindow->queryInterface<system::Win32Window>();
			    ::SetCapture( win32Window->mNativeData.hwnd );
				::ShowCursor( FALSE );
			}
        #endif

			return sysWindow;
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return nullptr;
	}

} // namespace ts3::gpuapi
