
#include "win32WindowSystem.h"
#include "win32DisplaySystem.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

	namespace platform
	{

		// Registers custom window class in the system.
		void _win32RegisterWndClass( Win32WindowNativeData & pWindowNativeData );

		// Translates a common-level frame style into a Win32-specific style mask.
		DWORD _win32TranslateFrameStyle( EFrameStyle pStyle );

		// Translates the specified geometry definition for a new frame (create) into a Win32-specific format.
		Win32FrameGeometry _win32CheckFrameGeometry( const FrameGeometry & pFrameGeometry );

		// Checks frame geometry/style definition which is to be applied to an existing frame (update).
		// Returns a Win32-specific geometry format, validated and adjusted to the selected style.
		Win32FrameGeometry _win32CheckFrameGeometryUpdate( HWND pWindowHwnd,
		                                                   const FrameGeometry & pFrameGeometry,
		                                                   Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags );

	}


	Win32WindowManager::Win32WindowManager( Win32DisplayManagerHandle pDisplayManager )
	: WindowManager( std::move( pDisplayManager ) )
	{}

	Win32WindowManager::~Win32WindowManager() noexcept = default;

	WindowHandle Win32WindowManager::_nativeCreateWindow( WindowCreateInfo pCreateInfo )
	{
		auto windowObject = createSysObject<Win32Window>( getHandle<Win32WindowManager>() );

		platform::win32CreateWindow( windowObject->mNativeData, pCreateInfo );

		::ShowWindow( windowObject->mNativeData.hwnd, SW_SHOWNORMAL );

		return windowObject;
	}


	Win32Window::Win32Window( Win32WindowManagerHandle pWindowManager )
	: Win32NativeObject( std::move( pWindowManager ) )
	{}

	Win32Window::~Win32Window() noexcept = default;

	void Win32Window::_nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode )
	{}

	void Win32Window::_nativeSetFullscreenMode( bool pEnable )
	{}

	void Win32Window::_nativeSetTitle( const std::string & pTitle )
	{
		platform::win32SetFrameTitle( mNativeData.hwnd, pTitle );
	}

	void Win32Window::_nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
	                                         Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
	{
		platform::win32UpdateFrameGeometry( mNativeData.hwnd, pFrameGeometry, pUpdateFlags );
	}

	FrameSize Win32Window::_nativeGetSize( EFrameSizeMode pSizeMode ) const
	{
		return platform::win32GetFrameSize( mNativeData.hwnd, pSizeMode );
	}


	namespace platform
	{

		void win32CreateWindow( Win32WindowNativeData & pWindowNativeData, const WindowCreateInfo & pCreateInfo )
		{
			// Register window class. Will fetch it if already registered.
			_win32RegisterWndClass( pWindowNativeData );

			auto geometryUpdateFlags = E_FRAME_GEOMETRY_UPDATE_FLAGS_ALL_DEFAULT;
			auto win32FrameGeometry = _win32CheckFrameGeometry( pCreateInfo.frameGeometry );

			HWND windowHwnd = ::CreateWindowExA( WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW,
			                                     pWindowNativeData.wndClsName,
			                                     pCreateInfo.title.c_str(),
			                                     win32FrameGeometry.style,
			                                     win32FrameGeometry.frameRect.left,
			                                     win32FrameGeometry.frameRect.top,
			                                     win32FrameGeometry.frameRect.right,
			                                     win32FrameGeometry.frameRect.bottom,
			                                     nullptr,
			                                     nullptr,
			                                     pWindowNativeData.moduleHandle,
			                                     &pWindowNativeData );

			if( windowHwnd == nullptr )
			{
				ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
			}

			::SetWindowPos( windowHwnd,
			                nullptr,
			                win32FrameGeometry.frameRect.left,
			                win32FrameGeometry.frameRect.top,
			                0, // Width of the frame
			                0, // Height of the frame
			                SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );

			// Retrieve the current number of windows created with our class (ClassRefCounter).
			auto wndClassRefCounter = ::GetClassLongPtrA( windowHwnd, 0 );

			// Increment the counter and store the new value.
			::SetClassLongPtrA( windowHwnd, 0, wndClassRefCounter + 1 );

			pWindowNativeData.hwnd = windowHwnd;
		}

		void win32DestroyWindow( Win32WindowNativeData & pWindowNativeData )
		{
			if( !pWindowNativeData.hwnd )
			{
				return;
			}

			// Retrieve the current number of windows created with our class (ClassRefCounter).
			auto wndClassRefCounter = ::GetClassLongPtrA( pWindowNativeData.hwnd, 0 );

			// We are destroying one of the windows, decrement the counter.
			--wndClassRefCounter;

			::SetClassLongPtrA( pWindowNativeData.hwnd, 0, wndClassRefCounter );

			::DestroyWindow( pWindowNativeData.hwnd );

			if( wndClassRefCounter == 0 )
			{
				::UnregisterClassA( pWindowNativeData.wndClsName, pWindowNativeData.moduleHandle );
			}

			pWindowNativeData.hwnd = nullptr;
			pWindowNativeData.wndClsID = 0;
			pWindowNativeData.wndClsName = nullptr;
			pWindowNativeData.moduleHandle = nullptr;
		}

		void win32SetFrameTitle( HWND pHWND, const std::string & pTitle )
		{
			::SetWindowTextA( pHWND, pTitle.c_str() );

			::SetWindowPos( pHWND, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW );
		}

		void win32UpdateFrameGeometry( HWND pHWND, const FrameGeometry & pFrameGeometry, Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
		{
			auto win32Geometry = platform::_win32CheckFrameGeometryUpdate( pHWND, pFrameGeometry, pUpdateFlags );

			::SetWindowLongA( pHWND, GWL_STYLE, win32Geometry.style );

			::SetWindowPos( pHWND,
			                nullptr,
			                win32Geometry.frameRect.left, // X coordinate
			                win32Geometry.frameRect.top, // Y coordinate
			                win32Geometry.frameRect.right, // Width of the frame
			                win32Geometry.frameRect.bottom, // Height of the frame
			                SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW );
		}

		FrameSize win32GetFrameSize( HWND pHWND, EFrameSizeMode pSizeMode )
		{
			RECT frameRect;
			if( pSizeMode == EFrameSizeMode::ClientArea )
			{
				::GetClientRect( pHWND, &frameRect );
			}
			else
			{
				::GetWindowRect( pHWND, &frameRect );
			}

			FrameSize resultFrameSize;
			resultFrameSize.x = frameRect.right - frameRect.left;
			resultFrameSize.y = frameRect.bottom - frameRect.top;

			return resultFrameSize;
		}

		void _win32RegisterWndClass( Win32WindowNativeData & pWindowNativeData )
		{
			//
			const LPCSTR wndClassName = "TS3_SYSTEM_WND_CLS";
			//
			const HMODULE wndProcModuleHandle = ::GetModuleHandleA( nullptr );

			WNDCLASSEXA windowClass;
			windowClass.cbSize = sizeof( WNDCLASSEXA );

			//
			BOOL classFindResult = ::GetClassInfoExA( wndProcModuleHandle, wndClassName, &windowClass );

			if( classFindResult == FALSE )
			{
				windowClass.cbSize = sizeof( WNDCLASSEXA );
				// Note this! We need one integer value for a simple ref counter.
				windowClass.cbClsExtra = sizeof( LONG_PTR );
				windowClass.cbWndExtra = 0;
				windowClass.hbrBackground = static_cast<HBRUSH>( ::GetStockObject( GRAY_BRUSH ) );
				windowClass.hCursor = ::LoadCursorA( nullptr, IDC_ARROW );
				windowClass.hIcon = ::LoadIconA( nullptr, IDI_WINLOGO );
				windowClass.hIconSm = ::LoadIconA( nullptr, IDI_WINLOGO );
				windowClass.hInstance = wndProcModuleHandle;
				windowClass.lpszClassName = wndClassName;
				windowClass.lpfnWndProc = win32DefaultWindowEventCallback;
				windowClass.lpszMenuName = nullptr;
				windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

				//
				auto wndClassID = ::RegisterClassExA( &windowClass );
				if( wndClassID == 0 )
				{
					ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
				}

				pWindowNativeData.wndClsID = wndClassID;
			}

			pWindowNativeData.wndClsName = wndClassName;
			pWindowNativeData.moduleHandle = wndProcModuleHandle;
		}

		DWORD _win32TranslateFrameStyle( EFrameStyle pStyle )
		{
			//
			constexpr DWORD cvCaptionFrameStyle = WS_CAPTION | WS_SYSMENU;
			//
			constexpr DWORD cvFixedFrameStyle = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED | WS_MINIMIZEBOX;
			//
			constexpr DWORD cvOverlayFrameStyle = WS_POPUP | WS_EX_TOPMOST;
			//
			constexpr DWORD cvResizeableFrameStyle = cvFixedFrameStyle | WS_SIZEBOX | WS_MAXIMIZEBOX;

			//
			DWORD resultStyle = Limits<DWORD>::maxValue;

			switch ( pStyle )
			{
				case EFrameStyle::Caption:
				{
					resultStyle = cvCaptionFrameStyle;
					break;
				}
				case EFrameStyle::Fixed:
				{
					resultStyle = cvFixedFrameStyle;
					break;
				}
				case EFrameStyle::Overlay:
				{
					resultStyle = cvOverlayFrameStyle;
					break;
				}
				case EFrameStyle::Resizeable:
				{
					resultStyle = cvResizeableFrameStyle;
					break;
				}
				default:
				{
					resultStyle = 0u;
					break;
				}
			}

			return resultStyle;
		}

		Win32FrameGeometry _win32CheckFrameGeometry( const FrameGeometry & pFrameGeometry )
		{
			Win32FrameGeometry win32Geometry;
			win32Geometry.frameRect.left = static_cast<LONG>( pFrameGeometry.position.x );
			win32Geometry.frameRect.top = static_cast<LONG>( pFrameGeometry.position.y );
			win32Geometry.frameRect.right = win32Geometry.frameRect.left + static_cast<LONG>( pFrameGeometry.size.x );
			win32Geometry.frameRect.bottom = win32Geometry.frameRect.top + static_cast<LONG>( pFrameGeometry.size.y );
			win32Geometry.style = _win32TranslateFrameStyle( pFrameGeometry.style );

			::AdjustWindowRect( &( win32Geometry.frameRect ), win32Geometry.style, FALSE );

			return win32Geometry;
		}

		Win32FrameGeometry _win32CheckFrameGeometryUpdate( HWND pWindowHwnd,
		                                                   const FrameGeometry & pFrameGeometry,
		                                                   Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
		{
			Win32FrameGeometry win32Geometry;

			if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_STYLE_BIT ) )
			{
				// If the style-related flag is set, it means the window style will change.
				// Current style is irrelevant - translate the new one and save it in result.
				win32Geometry.style = _win32TranslateFrameStyle( pFrameGeometry.style );
			}
			else
			{
				win32Geometry.style = ::GetWindowLongA( pWindowHwnd, GWL_STYLE );
			}

			if( pUpdateFlags.isSetAnyOf( E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_BIT ) )
			{
				RECT currentRect;
				::GetWindowRect( pWindowHwnd, &currentRect );

				auto framePosX = currentRect.left;
				auto framePosY = currentRect.top;
				auto frameWidth = currentRect.right - currentRect.left;
				auto frameHeight = currentRect.bottom - currentRect.top;

				if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT ) )
				{
					framePosX = static_cast<LONG>( pFrameGeometry.position.x );
					framePosY = static_cast<LONG>( pFrameGeometry.position.y );
				}
				if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_BIT ) )
				{
					frameWidth = static_cast<LONG>( pFrameGeometry.size.x );
					frameHeight = static_cast<LONG>( pFrameGeometry.size.y );
				}

				win32Geometry.frameRect.left = framePosX;
				win32Geometry.frameRect.top = framePosY;
				win32Geometry.frameRect.right = framePosX + frameWidth;
				win32Geometry.frameRect.bottom = framePosY + frameHeight;

				if( pUpdateFlags.isSet( E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_CLIENT_AREA_BIT ) )
				{
					::AdjustWindowRect( &( win32Geometry.frameRect ), win32Geometry.style, FALSE );
				}
			}
			else
			{
				::GetWindowRect( pWindowHwnd, &( win32Geometry.frameRect ) );
			}

			return win32Geometry;
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
