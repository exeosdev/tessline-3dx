
#include "x11WindowSystem.h"
#include "x11DisplaySystem.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

	namespace platform
	{

		std::vector<Atom> _x11QueryActionTableForFrameStyle( XDisplay pDisplay, EFrameStyle pFrameStyle );

		XSizeHints _x11QuerySizeHintsForFrameStyle( XDisplay pDisplay, EFrameStyle pFrameStyle, const FrameGeometry & pFrameGeometry );

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
		x11CreateInfo.frameGeometry = pCreateInfo.frameGeometry;
		x11CreateInfo.title = std::move( pCreateInfo.title );
		x11CreateInfo.colorDepth = XDefaultDepth( xSessionData.display, xSessionData.screenIndex );
		x11CreateInfo.windowVisual = XDefaultVisual( xSessionData.display, xSessionData.screenIndex );

		platform::x11CreateWindow( windowObject->mNativeData, x11CreateInfo );

		platform::x11WindowPostCreateUpdate( windowObject->mNativeData, x11CreateInfo );

		return windowObject;
	}

	void X11WindowManager::_nativeDestroyWindow( Window & pWindow )
	{
		auto * x11Window = pWindow.queryInterface<X11Window>();

		x11WindowPreDestroyUpdate( x11Window->mNativeData );

		x11DestroyWindow( x11Window->mNativeData );
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
			// weird stuff and this is ignored in most cases. XMoveWindow() together with XSetNormalHints() do the thing.
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
				ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
			}

			pWindowNativeData.windowXID = windowXID;
			pWindowNativeData.xColormap = colormap;

			// Window style requested for the window. There is no real "X11-equivalent" (like we have
			// on other OSes), because of the separation between X Server and the actual Window Manager.
			const auto windowStyle = pCreateInfo.frameGeometry.style;

			// First thing to enforce requested window behaviour: action table. It defines what kind of actions
			// (like moving, resizing, entering fullscreen) are allowed for a given window. Based on the style,
			// we query the table and set it for our newly created window.
			auto windowActionTable = platform::_x11QueryActionTableForFrameStyle( xSessionData.display, windowStyle );

			// This could happen only if we added/changed something and forgot to adjust this code.
			ts3DebugAssert( !windowActionTable.empty() );

			// The atom used to set this table is named "_NET_WM_ALLOWED_ACTIONS".
			Atom wmAllowedActions = XInternAtom( xSessionData.display, "_NET_WM_ALLOWED_ACTIONS", true );

			XChangeProperty( xSessionData.display,
			                 windowXID,
			                 wmAllowedActions,
			                 XA_ATOM,
			                 32,
			                 PropModeReplace,
			                 ( unsigned char * )windowActionTable.data(),
			                 ( int )windowActionTable.size() );

			if( pCreateInfo.fullscreenMode )
			{
				x11SetWindowFullscreenState( pWindowNativeData, true );
			}

			auto windowSizeHints = platform::_x11QuerySizeHintsForFrameStyle( xSessionData.display, windowStyle, pCreateInfo.frameGeometry );

			windowSizeHints.flags |= PPosition;
			windowSizeHints.x = static_cast<int>( pCreateInfo.frameGeometry.position.x );
			windowSizeHints.y = static_cast<int>( pCreateInfo.frameGeometry.position.y );

			// Note, that this call must be done before the window is mapped to the screen. Otherwise, most WMs
			// will not recognize PMinSize/PMaxSize hints, making non-resizable windows very much resizable ones.
			XSetNormalHints( xSessionData.display, windowXID, &windowSizeHints );
		}

		void x11WindowPostCreateUpdate( X11WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo )
		{
			auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );
			
			XMoveWindow( xSessionData.display,
						 pWindowNativeData.windowXID,
						 static_cast<int>( pCreateInfo.frameGeometry.position.x ),
						 static_cast<int>( pCreateInfo.frameGeometry.position.y ) );

			// Another observation: if this gets called before the initial XMoveWindow(), fullscreen windows
			// appear at a wrong position (and, thus, usually - on the wrong screen as well). Confirmed at least
			// on Kubuntu 20.04 and Mint. Non-fullscreen windows seem to work regardless of the call order.
			XMapWindow( xSessionData.display, pWindowNativeData.windowXID );

			Atom registeredWMProtocolArray[] =
			{
				xSessionData.wmpDeleteWindow,
				xSessionData.wmpDestroyWindow
			};
			const auto registeredWMProtocolsNum = staticArraySize( registeredWMProtocolArray );

			XSetWMProtocols( xSessionData.display,
			                 pWindowNativeData.windowXID,
			                 &( registeredWMProtocolArray[0] ),
			                 trunc_numeric_cast<int>( registeredWMProtocolsNum ) );

			XStoreName( xSessionData.display, pWindowNativeData.windowXID, pCreateInfo.title.c_str() );
			XFlush( xSessionData.display );
		}

		void x11WindowPreDestroyUpdate( X11WindowNativeData & pWindowNativeData )
		{
			auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

			XUnmapWindow( xSessionData.display, pWindowNativeData.windowXID );

			XFlush( xSessionData.display );
		}

		void x11DestroyWindow( X11WindowNativeData & pWindowNativeData )
		{
			auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

			XDestroyWindow( xSessionData.display, pWindowNativeData.windowXID );
			pWindowNativeData.windowXID = E_X11_XID_NONE;

			XFreeColormap( xSessionData.display, pWindowNativeData.xColormap );
			pWindowNativeData.xColormap = E_X11_XID_NONE;
		}

		void x11SetWindowFullscreenState( X11WindowNativeData & pWindowNativeData, bool pSetFullscreen )
		{
			auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

			// Fullscreen state of the window can be controlled by using _NET_WM_STATE property, containing a list
			// of atoms describing current state of a window. WM is required to respect this setting and provide
			// an internal mechanism for restoring the correct window geometry after the FS state is deactivated.

			Atom wmState = XInternAtom( xSessionData.display, "_NET_WM_STATE", true );

			Atom wmStateValueArray[] = { 0 };

			int32 wmStateValuesNum = 0;

			if( pSetFullscreen )
			{
				// The atom _NET_WM_STATE_FULLSCREEN indicates, that our window is currently is fullscreen mode.
				Atom wmFullscreen = XInternAtom( xSessionData.display, "_NET_WM_STATE_FULLSCREEN", true );

				wmStateValueArray[0] = wmFullscreen;
				wmStateValuesNum = 1;
			}

			XChangeProperty( xSessionData.display,
							 pWindowNativeData.windowXID,
							 wmState,
							 XA_ATOM,
							 32,
							 PropModeReplace,
							 ( unsigned char * )( &( wmStateValueArray[0] ) ),
							 wmStateValuesNum );
		}

		void x11SetFrameTitle( const X11WindowNativeData & pWindowNativeData, const std::string & pTitle )
		{
			auto & xSessionData = platform::x11GetXSessionData( pWindowNativeData );

			XStoreName( xSessionData.display, pWindowNativeData.windowXID, pTitle.c_str() );

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

		XSizeHints _x11QuerySizeHintsForFrameStyle( XDisplay pDisplay, EFrameStyle pFrameStyle, const FrameGeometry & pFrameGeometry )
		{
			XSizeHints windowSizeHints;
			windowSizeHints.flags = PSize;
			windowSizeHints.width = static_cast<int>( pFrameGeometry.size.x );
			windowSizeHints.height = static_cast<int>( pFrameGeometry.size.y );

			if( pFrameStyle != EFrameStyle::Resizeable )
			{
				windowSizeHints.flags |= PMaxSize;
				windowSizeHints.max_width = windowSizeHints.width;
				windowSizeHints.max_height = windowSizeHints.height;

				windowSizeHints.flags |= PMinSize;
				windowSizeHints.min_width = windowSizeHints.width;
				windowSizeHints.min_height = windowSizeHints.height;
			}

			return windowSizeHints;
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
