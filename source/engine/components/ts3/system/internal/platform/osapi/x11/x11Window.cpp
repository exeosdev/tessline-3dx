
#include <ts3/system/windowSystem.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3
{

	void SysWindowManager::_sysInitialize()
	{
		auto & scNativeData = mSysContext->mNativeData;
		scNativeData.wmpDeleteWindow = XInternAtom( scNativeData.display, "WM_DELETE_WINDOW", False );
	}

	void SysWindowManager::_sysRelease() noexcept
	{}


	void SysWindow::_sysInitialize( const SysWindowCreateInfo & pCreateInfo )
	{
		auto & scNativeData = mSysContext->mNativeData;

		SysX11WindowCreateInfo x11WindowCreateInfo;
		x11WindowCreateInfo.commonProperties = pCreateInfo.properties;
		x11WindowCreateInfo.display = scNativeData.display;
		x11WindowCreateInfo.screenIndex = scNativeData.screenIndex;
		x11WindowCreateInfo.rootWindow = scNativeData.rootWindow;
		x11WindowCreateInfo.wmpDeleteWindow = scNativeData.wmpDeleteWindow;
		x11WindowCreateInfo.colorDepth = XDefaultDepth( scNativeData.display, scNativeData.screenIndex );
		x11WindowCreateInfo.windowVisual = XDefaultVisual( scNativeData.display, scNativeData.screenIndex );

		sysX11CreateWindow( mNativeData, x11WindowCreateInfo );
		sysX11UpdateNewWindowState( mNativeData, x11WindowCreateInfo );
	}

	void SysWindow::_sysRelease() noexcept
	{}

	void SysWindow::_sysGetClientAreaSize( SysWindowSize & pClientAreaSize ) const
	{
		XWindowAttributes windowAttributes;
		XGetWindowAttributes( mNativeData.display,
		                      mNativeData.xwindow,
		                      &windowAttributes );

		pClientAreaSize.x = windowAttributes.width - windowAttributes.border_width;
		pClientAreaSize.y = windowAttributes.height - windowAttributes.border_width;
	}

	void SysWindow::_sysGetFrameSize( SysWindowSize & pFrameSize ) const
	{
		XWindowAttributes windowAttributes;
		XGetWindowAttributes( mNativeData.display,
		                      mNativeData.xwindow,
		                      &windowAttributes );

		pFrameSize.x = windowAttributes.width;
		pFrameSize.y = windowAttributes.height;
	}


	void sysX11CreateWindow( SysWindowNativeData & pWindowNativeData, const SysX11WindowCreateInfo & pCreateInfo )
	{
		// Colormap for our window.
		Colormap colormap = XCreateColormap( pCreateInfo.display,
		                                     pCreateInfo.rootWindow,
		                                     pCreateInfo.windowVisual,
		                                     AllocNone );

		// Mask of all event types we want to register for and handle.
		constexpr long windowEventMask = FocusChangeMask | PropertyChangeMask| ExposureMask |
		                                 StructureNotifyMask | KeyPressMask | KeyReleaseMask |
		                                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		                                 Button1MotionMask | Button2MotionMask | Button3MotionMask |
		                                 Button4MotionMask | Button5MotionMask | ButtonMotionMask;

		XSetWindowAttributes windowSetAttributes;
		windowSetAttributes.background_pixel = 0;
		windowSetAttributes.border_pixel = 0;
		windowSetAttributes.colormap = colormap;
		windowSetAttributes.event_mask = windowEventMask;
		// Mask for all attributes we want to specify (fields in XSetWindowAttributes filled above).
		unsigned long windowSetAttributesMask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

		// We pass initial position as (0,0) instead of the specified one, because window managers tend to do some
		// weird stuff and this is ignored in most cases. XMoveWindow (after window gets mapped) does the right thing.
		Window xwindow = XCreateWindow( pCreateInfo.display,
		                                pCreateInfo.rootWindow,
		                                0, 0,
		                                pCreateInfo.commonProperties.geometry.size.x,
		                                pCreateInfo.commonProperties.geometry.size.y,
		                                0u,
		                                pCreateInfo.colorDepth,
		                                InputOutput,
		                                pCreateInfo.windowVisual,
		                                windowSetAttributesMask,
		                                &windowSetAttributes );

		if( xwindow == cvXIDNone )
		{
			throw 0;
		}

		if( pCreateInfo.fullscreenMode )
		{
			Atom wmState = XInternAtom( pCreateInfo.display, "_NET_WM_STATE", true );
			Atom wmFullscreen = XInternAtom( pCreateInfo.display, "_NET_WM_STATE_FULLSCREEN", true );

			XChangeProperty( pCreateInfo.display,
			                 xwindow,
			                 wmState,
			                 XA_ATOM,
			                 32,
			                 PropModeReplace,
			                 (unsigned char *)&wmFullscreen,
			                 1 );
		}

		pWindowNativeData.display = pCreateInfo.display;
		pWindowNativeData.xwindow = xwindow;
		pWindowNativeData.colormap = colormap;
	}

	void sysX11UpdateNewWindowState( SysWindowNativeData & pWindowNativeData, const SysX11WindowCreateInfo & pCreateInfo )
	{
		XMapWindow( pWindowNativeData.display, pWindowNativeData.xwindow );

		XMoveWindow( pWindowNativeData.display,
		             pWindowNativeData.xwindow,
		             pCreateInfo.commonProperties.geometry.position.x,
		             pCreateInfo.commonProperties.geometry.position.y );

		Atom wmpDeleteWindowProtocol = pCreateInfo.wmpDeleteWindow;
		XSetWMProtocols( pWindowNativeData.display, pWindowNativeData.xwindow, &wmpDeleteWindowProtocol, 1 );
		XStoreName( pWindowNativeData.display, pWindowNativeData.xwindow, pCreateInfo.commonProperties.title.c_str() );
		XFlush( pWindowNativeData.display );
	}

	void sysX11DestroyWindow( SysWindowNativeData & pWindowNativeData )
	{
		XDestroyWindow( pWindowNativeData.display, pWindowNativeData.xwindow );
		pWindowNativeData.xwindow = cvXIDNone;
		XFreeColormap( pWindowNativeData.display, pWindowNativeData.colormap );
		pWindowNativeData.colormap = cvXIDNone;
	}

}
#endif
