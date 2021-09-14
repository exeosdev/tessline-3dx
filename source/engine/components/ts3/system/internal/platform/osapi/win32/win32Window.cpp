
#include <ts3/system/windowNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

	void _win32RegisterWndClass( WindowNativeData & pWindowNativeData );
	DWORD _win32TranslateFrameStyle( WindowFrameStyle pStyle );
	LRESULT __stdcall _win32DefaultWindowEventCallback( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );


	void WindowManager::_nativeCreateWindow( Window & pWindow, const WindowCreateInfo & pCreateInfo )
	{
	    nativeWin32CreateWindow( pWindow.mPrivate->nativeDataPriv, pCreateInfo );

	    ::ShowWindow( pWindow.mPrivate->nativeDataPriv.hwnd, SW_SHOWNORMAL );
	}

	void WindowManager::_nativeDestroyWindow( Window & pWindow )
	{
	    nativeWin32DestroyWindow( pWindow.mPrivate->nativeDataPriv );
	}

	void WindowManager::_nativeRemoveWindow( Window & pWindow )
	{
	    // TODO
	}


	void Window::_nativeResize( const WindowSize & pNewWindowSize, WindowSizeMode pSizeMode )
	{
	    RECT windowRect;
	    windowRect.left = 0;
	    windowRect.top = 0;
	    windowRect.right = pNewWindowSize.x;
	    windowRect.bottom = pNewWindowSize.y;

	    if( pSizeMode == WindowSizeMode::ClientArea )
	    {
	        auto windowStyle = ::GetWindowLongA( mNativeData->hwnd, GWL_STYLE );
	        ::AdjustWindowRect( &windowRect, windowStyle, FALSE );
	    }

	    ::SetWindowPos( mNativeData->hwnd,
                        nullptr,
                        0, // X coordinate
                        0, // Y coordinate
                        static_cast<int>( windowRect.right - windowRect.left ), // Width of the frame
                        static_cast<int>( windowRect.bottom - windowRect.top ), // Height of the frame
                        SWP_NOZORDER | SWP_FRAMECHANGED );
	}

	void Window::_nativeSetTitleText( const std::string & pTitleText )
	{}

	void Window::_nativeUpdateGeometry( const WindowGeometry & pWindowGeometry )
	{}

	void Window::_nativeGetSize( WindowSizeMode pSizeMode, WindowSize & pOutSize ) const
	{
	    RECT resultRect;

	    if( pSizeMode == WindowSizeMode::ClientArea )
	    {
	        ::GetClientRect( mNativeData->hwnd, &resultRect );
	    }
	    else
	    {
	        ::GetWindowRect( mNativeData->hwnd, &resultRect );
	    }

	    pOutSize.x = resultRect.right - resultRect.left;
	    pOutSize.y = resultRect.bottom - resultRect.top;
	}


	void nativeWin32CreateWindow( WindowNativeData & pWindowNativeData, const WindowCreateInfo & pCreateInfo )
	{
		// Register window class. Will fetch it if already registered.
		_win32RegisterWndClass( pWindowNativeData );

		RECT windowRect;
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = static_cast<LONG>( pCreateInfo.properties.geometry.size.x );
		windowRect.bottom = static_cast<LONG>( pCreateInfo.properties.geometry.size.y );

		auto win32FrameStyle = _win32TranslateFrameStyle( pCreateInfo.properties.geometry.frameStyle );
		::AdjustWindowRect( &windowRect, win32FrameStyle, FALSE );

		auto framePosX = pCreateInfo.properties.geometry.position.x;
		auto framePosY = pCreateInfo.properties.geometry.position.y;
		auto frameWidth = windowRect.right - windowRect.left;
		auto frameHeight = windowRect.bottom - windowRect.top;

		HWND windowHwnd = ::CreateWindowExA( WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW,
                                             pWindowNativeData.wndClsName,
                                             pCreateInfo.properties.title.c_str(),
                                             win32FrameStyle,
                                             framePosX,
                                             framePosY,
                                             frameWidth,
                                             frameHeight,
                                             nullptr,
                                             nullptr,
                                             pWindowNativeData.moduleHandle,
                                             &pWindowNativeData );

		if ( windowHwnd == nullptr )
		{
			throw 0;
		}

		::SetWindowPos( windowHwnd,
                        nullptr,
                        0, // X coordinate
                        0, // Y coordinate
                        0, // Width of the frame
                        0, // Height of the frame
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );

		// Retrieve the current number of windows created with our class (ClassRefCounter).
		auto wndClassRefCounter = ::GetClassLongPtrA( windowHwnd, 0 );

		// Increment the counter and store the new value.
		::SetClassLongPtrA( windowHwnd, 0, wndClassRefCounter + 1 );

		pWindowNativeData.hwnd = windowHwnd;
	}

	void nativeWin32DestroyWindow( WindowNativeData & pWindowNativeData )
	{
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

	void _win32RegisterWndClass( WindowNativeData & pWindowNativeData )
	{
		//
		const LPCSTR wndClassName = "TS3_SYSTEM_WNDCLS";
		//
		const HMODULE wndProcModuleHandle = ::GetModuleHandleA( nullptr );

		WNDCLASSEXA windowClass;
		windowClass.cbSize = sizeof( WNDCLASSEXA );

		//
		BOOL classFindResult = ::GetClassInfoExA( wndProcModuleHandle, wndClassName, &windowClass );

		if ( classFindResult == FALSE )
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
			windowClass.lpfnWndProc = _win32DefaultWindowEventCallback;
			windowClass.lpszMenuName = nullptr;
			windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

			//
			auto wndClassID = ::RegisterClassExA( &windowClass );
			if ( wndClassID == 0 )
			{
				throw 0;
			}

			pWindowNativeData.wndClsID = wndClassID;
		}

		pWindowNativeData.wndClsName = wndClassName;
		pWindowNativeData.moduleHandle = wndProcModuleHandle;
	}

	DWORD _win32TranslateFrameStyle( WindowFrameStyle pStyle )
	{
		//
		constexpr DWORD overlayFrameStyle = WS_POPUP;
		//
		constexpr DWORD captionFrameStyle = WS_CAPTION | WS_SYSMENU;
		//
		constexpr DWORD fixedFrameStyle = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED | WS_MINIMIZEBOX;
		//
		constexpr DWORD resizeableFrameStyle = fixedFrameStyle | WS_SIZEBOX | WS_MAXIMIZEBOX;

		//
		DWORD resultStyle = 0U;

		switch ( pStyle )
		{
			case WindowFrameStyle::Caption:
				resultStyle = captionFrameStyle;
				break;

			case WindowFrameStyle::Fixed:
				resultStyle = fixedFrameStyle;
				break;

			case WindowFrameStyle::Overlay:
				resultStyle = overlayFrameStyle;
				break;

			case WindowFrameStyle::Resizeable:
				resultStyle = resizeableFrameStyle;
				break;

			default:
				resultStyle = fixedFrameStyle;
				break;
		}

		return resultStyle;
	}

	LRESULT __stdcall _win32DefaultWindowEventCallback( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam )
	{
		return ::DefWindowProcA( pHWND, pMessage, pWparam, pLparam );
	}

} // namespace ts3::system
#endif
