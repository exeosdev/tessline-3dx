
#include <ts3/system/windowNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

    void WindowManager::_nativeCtor()
    {
        mInternal->nativeDataPriv.setSessionData( nativeX11GetXSessionData( *mSysContext ) );
    }

    void WindowManager::_nativeDtor() noexcept
    {
        mInternal->nativeDataPriv.resetSessionData();
    }

    void WindowManager::_nativeCreateWindow( Window & pWindow, const WindowCreateInfo & pCreateInfo )
    {
        auto & xSessionData = nativeX11GetXSessionDataInternal( *this );

        X11WindowCreateInfo x11CreateInfo;
        x11CreateInfo.setSessionData( xSessionData );
        x11CreateInfo.commonProperties = pCreateInfo.properties;
        x11CreateInfo.colorDepth = XDefaultDepth( xSessionData.display, xSessionData.screenIndex );
        x11CreateInfo.windowVisual = XDefaultVisual( xSessionData.display, xSessionData.screenIndex );

        nativeX11CreateWindow( pWindow.mInternal->nativeDataPriv, x11CreateInfo );

        nativeX11UpdateNewWindowState( pWindow.mInternal->nativeDataPriv, x11CreateInfo );
    }

    void WindowManager::_nativeDestroyWindow( Window & pWindow )
    {
        nativeX11DestroyWindow( pWindow.mInternal->nativeDataPriv );
    }

    bool WindowManager::_nativeIsWindowValid( const Window & pWindow ) const noexcept
	{
        return pWindow.mInternal->nativeDataPriv.windowXID != E_XID_NONE;
	}


    void Window::_nativeSetTitleText( const std::string & pTitleText )
    {
        auto & xSessionData = nativeX11GetXSessionDataInternal( *this );

        XStoreName( xSessionData.display,
                    mInternal->nativeDataPriv.windowXID,
                    pTitleText.c_str() );

        XFlush( xSessionData.display );
    }

    void Window::_nativeUpdateGeometry( const WindowGeometry & pWindowGeometry, WindowSizeMode pSizeMode )
    {
        auto & xSessionData = nativeX11GetXSessionDataInternal( *this );

        XMoveWindow( xSessionData.display,
                     mInternal->nativeDataPriv.windowXID,
                     pWindowGeometry.position.x,
                     pWindowGeometry.position.y );

        XResizeWindow( xSessionData.display,
                       mInternal->nativeDataPriv.windowXID,
                       pWindowGeometry.size.x,
                       pWindowGeometry.size.y );

        XFlush( xSessionData.display );
    }

    void Window::_nativeGetSize( WindowSizeMode pSizeMode, WindowSize & pOutSize ) const
	{
        auto & xSessionData = nativeX11GetXSessionDataInternal( *this );

		XWindowAttributes windowAttributes;
		XGetWindowAttributes( xSessionData.display,
                              mInternal->nativeDataPriv.windowXID,
		                      &windowAttributes );

        if( pSizeMode == WindowSizeMode::ClientArea )
        {
            pOutSize.x = windowAttributes.width - windowAttributes.border_width;
            pOutSize.y = windowAttributes.height - windowAttributes.border_width;
        }
        else
        {
            pOutSize.x = windowAttributes.width;
            pOutSize.y = windowAttributes.height;
        }
	}


	void nativeX11CreateWindow( WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo )
	{
        auto & xSessionData = nativeX11GetXSessionData( pCreateInfo );

		// Colormap for our window.
		Colormap colormap = XCreateColormap( xSessionData.display,
		                                     xSessionData.rootWindowXID,
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
		auto windowXID = XCreateWindow( xSessionData.display,
                                        xSessionData.rootWindowXID,
                                        0, 0,
                                        pCreateInfo.commonProperties.geometry.size.x,
                                        pCreateInfo.commonProperties.geometry.size.y,
                                        0u,
                                        pCreateInfo.colorDepth,
                                        InputOutput,
                                        pCreateInfo.windowVisual,
                                        windowSetAttributesMask,
                                        &windowSetAttributes );

		if( windowXID == E_X11_XID_NONE )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		if( pCreateInfo.fullscreenMode )
		{
			Atom wmState = XInternAtom( xSessionData.display, "_NET_WM_STATE", true );
			Atom wmFullscreen = XInternAtom( xSessionData.display, "_NET_WM_STATE_FULLSCREEN", true );

			XChangeProperty( xSessionData.display,
                             windowXID,
			                 wmState,
			                 XA_ATOM,
			                 32,
			                 PropModeReplace,
			                 (unsigned char *)&wmFullscreen,
			                 1 );
		}

        pWindowNativeData.setSessionData( xSessionData );
        pWindowNativeData.windowXID = windowXID;
		pWindowNativeData.xColormap = colormap;
	}

	void nativeX11UpdateNewWindowState( WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo )
	{
        auto & xSessionData = nativeX11GetXSessionData( pWindowNativeData );
        
		XMapWindow( xSessionData.display, pWindowNativeData.windowXID );

		XMoveWindow( xSessionData.display,
		             pWindowNativeData.windowXID,
		             pCreateInfo.commonProperties.geometry.position.x,
		             pCreateInfo.commonProperties.geometry.position.y );

		Atom wmpDeleteWindowProtocol = XInternAtom( xSessionData.display, "WM_DELETE_WINDOW", False );
		XSetWMProtocols( xSessionData.display, pWindowNativeData.windowXID, &wmpDeleteWindowProtocol, 1 );
		XStoreName( xSessionData.display, pWindowNativeData.windowXID, pCreateInfo.commonProperties.title.c_str() );
		XFlush( xSessionData.display );
	}

	void nativeX11DestroyWindow( WindowNativeData & pWindowNativeData )
	{
        auto & xSessionData = nativeX11GetXSessionData( pWindowNativeData );

		XDestroyWindow( xSessionData.display, pWindowNativeData.windowXID );
		pWindowNativeData.windowXID = E_X11_XID_NONE;

		XFreeColormap( xSessionData.display, pWindowNativeData.xColormap );
		pWindowNativeData.xColormap = E_X11_XID_NONE;

        pWindowNativeData.resetSessionData();
	}

} // namespace ts3::system
#endif
