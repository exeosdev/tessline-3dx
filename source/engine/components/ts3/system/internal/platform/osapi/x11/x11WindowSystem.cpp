
#include "x11WindowSystem.h"
#include "x11DisplaySystem.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

    namespace platform
    {

        std::vector<Atom> _x11QueryActionTableForFrameStyle( XDisplay pDisplay, EFrameStyle pFrameStyle );

    }
    
    X11WindowManager::X11WindowManager( X11DisplayManagerHandle pDisplayManager )
    : X11NativeObject( std::move( pDisplayManager ) )
    {}

    X11WindowManager::~X11WindowManager() noexcept = default;

    WindowHandle X11WindowManager::_nativeCreateWindow( WindowCreateInfo pCreateInfo )
    {
        auto & xSessionData = platform::x11GetXSessionData( *this );

        auto windowObject = createSysObject<X11Window>( getHandle<X11WindowManager>() );

        platform::X11WindowCreateInfo x11CreateInfo;
        x11CreateInfo.setSessionData( xSessionData );
        x11CreateInfo.frameGeometry = pCreateInfo.frameGeometry;
        x11CreateInfo.title = std::move( pCreateInfo.title );
        x11CreateInfo.colorDepth = XDefaultDepth( xSessionData.display, xSessionData.screenIndex );
        x11CreateInfo.windowVisual = XDefaultVisual( xSessionData.display, xSessionData.screenIndex );

        platform::x11CreateWindow( windowObject->mNativeData, x11CreateInfo );

        platform::x11UpdateNewWindowState( windowObject->mNativeData, x11CreateInfo );

        return windowObject;
    }
    

    X11Window::X11Window( X11WindowManagerHandle pWindowManager )
    : X11NativeObject( std::move( pWindowManager ), &mNativeData )
    {}

    X11Window::~X11Window() noexcept = default;

    void X11Window::_nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode )
    {}

    void X11Window::_nativeSetFullscreenMode( bool pEnable )
    {}

    void X11Window::_nativeSetTitle( const std::string & pTitle )
    {
        return platform::x11SetFrameTitle( mNativeData, pTitle );
    }

    void X11Window::_nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
                                           Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
    {
        return platform::x11UpdateFrameGeometry( mNativeData, pFrameGeometry, pUpdateFlags );
    }

    FrameSize X11Window::_nativeGetSize( EFrameSizeMode pSizeMode ) const
    {
        return platform::x11GetFrameSize( mNativeData, pSizeMode );
    }


    namespace platform
    {

        void x11CreateWindow( X11WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo )
        {
            auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

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
                                            pCreateInfo.frameGeometry.size.x,
                                            pCreateInfo.frameGeometry.size.y,
                                            0u,
                                            pCreateInfo.colorDepth,
                                            InputOutput,
                                            pCreateInfo.windowVisual,
                                            windowSetAttributesMask,
                                            &windowSetAttributes );

            if( windowXID == E_X11_XID_NONE )
            {
                ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            auto windowActionTable = platform::_x11QueryActionTableForFrameStyle( xSessionData.display,
                                                                                  pCreateInfo.frameGeometry.style );

            if( !windowActionTable.empty() )
            {
                Atom wmAllowedActions = XInternAtom( xSessionData.display, "_NET_WM_ALLOWED_ACTIONS", true );

                XChangeProperty( xSessionData.display,
                                 windowXID,
                                 wmAllowedActions,
                                 XA_ATOM,
                                 32,
                                 PropModeReplace,
                                 ( unsigned char * )windowActionTable.data(),
                                 ( int )windowActionTable.size() );
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
                                 ( unsigned char * )&wmFullscreen,
                                 1 );
            }

            pWindowNativeData.setSessionData( xSessionData );
            pWindowNativeData.windowXID = windowXID;
            pWindowNativeData.xColormap = colormap;
        }

        void x11UpdateNewWindowState( X11WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo )
        {
            auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

            XMapWindow( xSessionData.display, pWindowNativeData.windowXID );

            XMoveWindow( xSessionData.display,
                         pWindowNativeData.windowXID,
                         pCreateInfo.frameGeometry.position.x,
                         pCreateInfo.frameGeometry.position.y );

            Atom wmpDeleteWindowProtocol = XInternAtom( xSessionData.display, "WM_DELETE_WINDOW", False );
            XSetWMProtocols( xSessionData.display, pWindowNativeData.windowXID, &wmpDeleteWindowProtocol, 1 );
            XStoreName( xSessionData.display, pWindowNativeData.windowXID, pCreateInfo.title.c_str() );
            XFlush( xSessionData.display );
        }

        void x11DestroyWindow( X11WindowNativeData & pWindowNativeData )
        {
            auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

            XDestroyWindow( xSessionData.display, pWindowNativeData.windowXID );
            pWindowNativeData.windowXID = E_X11_XID_NONE;

            XFreeColormap( xSessionData.display, pWindowNativeData.xColormap );
            pWindowNativeData.xColormap = E_X11_XID_NONE;

            pWindowNativeData.resetSessionData();
        }

        void x11SetFrameTitle( const X11WindowNativeData & pWindowNativeData, const std::string & pTitle )
        {
            auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

            XStoreName( xSessionData.display,
                        pWindowNativeData.windowXID,
                        pTitle.c_str() );

            XFlush( xSessionData.display );
        }

        void x11UpdateFrameGeometry( const X11WindowNativeData & pWindowNativeData,
                                     const FrameGeometry & pFrameGeometry,
                                     Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
        {
            auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

            if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT ) )
            {
                XMoveWindow( xSessionData.display,
                             pWindowNativeData.windowXID,
                             pFrameGeometry.position.x,
                             pFrameGeometry.position.y );
            }

            if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_BIT ) )
            {
                XResizeWindow( xSessionData.display,
                               pWindowNativeData.windowXID,
                               pFrameGeometry.size.x,
                               pFrameGeometry.size.y );
            }

            XFlush( xSessionData.display );
        }

        FrameSize x11GetFrameSize( const X11WindowNativeData & pWindowNativeData, EFrameSizeMode pSizeMode )
        {
            auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

            XWindowAttributes windowAttributes;
            XGetWindowAttributes( xSessionData.display,
                                  pWindowNativeData.windowXID,
                                  &windowAttributes );

            FrameSize resultFrameSize;

            if( pSizeMode == EFrameSizeMode::ClientArea )
            {
                resultFrameSize.x = windowAttributes.width - windowAttributes.border_width;
                resultFrameSize.y = windowAttributes.height - windowAttributes.border_width;
            }
            else
            {
                resultFrameSize.x = windowAttributes.width;
                resultFrameSize.y = windowAttributes.height;
            }

            return resultFrameSize;
        }

        // List of Xlib-defined window actions:
        // _NET_WM_ACTION_CHANGE_DESKTOP
        // _NET_WM_ACTION_FULLSCREEN
        // _NET_WM_ACTION_MAXIMIZE_HORZ
        // _NET_WM_ACTION_MAXIMIZE_VERT
        // _NET_WM_ACTION_MINIMIZE
        // _NET_WM_ACTION_RESIZE
        // _NET_WM_ACTION_MOVE
        // _NET_WM_ACTION_CLOSE
        // _NET_WM_ACTION_STICK
        // _NET_WM_ACTION_SHADE

        static const char * sX11WindowActionTableCaption[] =
        {
            "_NET_WM_ACTION_CHANGE_DESKTOP",
            "_NET_WM_ACTION_FULLSCREEN",
            "_NET_WM_ACTION_MOVE",
            "_NET_WM_ACTION_STICK",
            "_NET_WM_ACTION_SHADE"
        };

        static const char * sX11WindowActionTableFixed[] =
        {
            "_NET_WM_ACTION_CHANGE_DESKTOP",
            "_NET_WM_ACTION_FULLSCREEN",
            "_NET_WM_ACTION_MINIMIZE",
            "_NET_WM_ACTION_MOVE",
            "_NET_WM_ACTION_CLOSE",
            "_NET_WM_ACTION_STICK",
            "_NET_WM_ACTION_SHADE"
        };

        static const char * sX11WindowActionTableOverlay[] =
        {
            "_NET_WM_ACTION_CHANGE_DESKTOP",
            "_NET_WM_ACTION_FULLSCREEN",
            "_NET_WM_ACTION_STICK",
            "_NET_WM_ACTION_SHADE"
        };

        static const char * sX11WindowActionTableResizeable[] =
        {
            "_NET_WM_ACTION_CHANGE_DESKTOP",
            "_NET_WM_ACTION_FULLSCREEN",
            "_NET_WM_ACTION_MAXIMIZE_HORZ",
            "_NET_WM_ACTION_MAXIMIZE_VERT",
            "_NET_WM_ACTION_MINIMIZE",
            "_NET_WM_ACTION_RESIZE",
            "_NET_WM_ACTION_MOVE",
            "_NET_WM_ACTION_CLOSE",
            "_NET_WM_ACTION_STICK",
            "_NET_WM_ACTION_SHADE"
        };

        std::vector<Atom> _x11QueryActionTableForFrameStyle( XDisplay pDisplay, EFrameStyle pFrameStyle )
        {
            const char ** windowActionTablePtr = nullptr;
            size_t windowActionTableSize = 0u;

            switch( pFrameStyle )
            {
                case EFrameStyle::Caption:
                    windowActionTablePtr = &( sX11WindowActionTableCaption[0] );
                    windowActionTableSize = staticArraySize( sX11WindowActionTableCaption );
                    break;

                case EFrameStyle::Fixed:
                    windowActionTablePtr = &( sX11WindowActionTableFixed[0] );
                    windowActionTableSize = staticArraySize( sX11WindowActionTableFixed );
                    break;

                case EFrameStyle::Overlay:
                    windowActionTablePtr = &( sX11WindowActionTableOverlay[0] );
                    windowActionTableSize = staticArraySize( sX11WindowActionTableOverlay );
                    break;

                case EFrameStyle::Resizeable:
                    windowActionTablePtr = &( sX11WindowActionTableResizeable[0] );
                    windowActionTableSize = staticArraySize( sX11WindowActionTableResizeable );
                    break;
            }

            std::vector<Atom> resultAtomTable;

            if( windowActionTablePtr )
            {
                resultAtomTable.resize( windowActionTableSize, 0u );

                for( size_t atomIndex = 0; atomIndex < windowActionTableSize; ++atomIndex )
                {
                    const auto * atomName = windowActionTablePtr[atomIndex];
                    resultAtomTable[atomIndex] = XInternAtom( pDisplay, atomName, true );
                }
            }

            return resultAtomTable;
        }

    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
