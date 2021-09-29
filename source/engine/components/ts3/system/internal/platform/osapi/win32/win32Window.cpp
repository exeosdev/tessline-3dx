
#include <ts3/system/windowNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

	void _win32RegisterWndClass( WindowNativeData & pWindowNativeData );
	DWORD _win32TranslateFrameStyle( EWindowFrameStyle pStyle );
	Win32WindowGeometry _win32CheckWindowGeometry( const WindowGeometry & pWindowGeometry, EWindowSizeMode pSizeMode, HWND pWindowHwnd );
	LRESULT __stdcall _win32DefaultWindowEventCallback( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );


	void WindowManager::_nativeConstructor()
    {}

    void WindowManager::_nativeDestructor() noexcept
    {}

	void WindowManager::_nativeCreateWindow( Window & pWindow, const WindowCreateInfo & pCreateInfo )
	{
	    nativeWin32CreateWindow( pWindow.mInternal->nativeDataPriv, pCreateInfo );

	    ::ShowWindow( pWindow.mInternal->nativeDataPriv.hwnd, SW_SHOWNORMAL );
	}

	void WindowManager::_nativeDestroyWindow( Window & pWindow )
	{
	    nativeWin32DestroyWindow( pWindow.mInternal->nativeDataPriv );
	}

	bool WindowManager::_nativeIsWindowValid( const Window & pWindow ) const noexcept
	{
	    return pWindow.mInternal->nativeDataPriv.hwnd != nullptr;
	}


	void Window::_nativeSetTitleText( const std::string & pTitleText )
	{}

	void Window::_nativeUpdateGeometry( const WindowGeometry & pWindowGeometry, EWindowSizeMode pSizeMode )
	{
	    auto win32Geometry = _win32CheckWindowGeometry( pWindowGeometry, pSizeMode, mNativeData->hwnd );

	    ::SetWindowLongPtrA( mNativeData->hwnd, GWL_STYLE, win32Geometry.style );

	    ::SetWindowPos( mNativeData->hwnd,
                        nullptr,
                        win32Geometry.frameRect.left, // X coordinate
                        win32Geometry.frameRect.top, // Y coordinate
                        win32Geometry.frameRect.right, // Width of the frame
                        win32Geometry.frameRect.bottom, // Height of the frame
                        SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW );
	}

	void Window::_nativeGetSize( EWindowSizeMode pSizeMode, WindowSize & pOutSize ) const
	{
	    RECT resultRect;

	    if( pSizeMode == EWindowSizeMode::ClientArea )
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

		auto win32Geometry = _win32CheckWindowGeometry( pCreateInfo.properties.geometry, EWindowSizeMode::ClientArea, nullptr );

		HWND windowHwnd = ::CreateWindowExA( WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW,
                                             pWindowNativeData.wndClsName,
                                             pCreateInfo.properties.title.c_str(),
                                             win32Geometry.style,
                                             win32Geometry.frameRect.left,
                                             win32Geometry.frameRect.top,
                                             win32Geometry.frameRect.right,
                                             win32Geometry.frameRect.bottom,
                                             nullptr,
                                             nullptr,
                                             pWindowNativeData.moduleHandle,
                                             &pWindowNativeData );

		if ( windowHwnd == nullptr )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		::SetWindowPos( windowHwnd,
                        nullptr,
                        win32Geometry.frameRect.left,
                        win32Geometry.frameRect.top,
                        0, // Width of the frame
                        0, // Height of the frame
                        SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );

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
		const LPCSTR wndClassName = "TS3_SYSTEM_WND_CLS";
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
				ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
			}

			pWindowNativeData.wndClsID = wndClassID;
		}

		pWindowNativeData.wndClsName = wndClassName;
		pWindowNativeData.moduleHandle = wndProcModuleHandle;
	}

	DWORD _win32TranslateFrameStyle( EWindowFrameStyle pStyle )
	{
		//
		constexpr DWORD overlayFrameStyle = WS_POPUP | WS_EX_TOPMOST;
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
			case EWindowFrameStyle::Caption:
				resultStyle = captionFrameStyle;
				break;

			case EWindowFrameStyle::Fixed:
				resultStyle = fixedFrameStyle;
				break;

			case EWindowFrameStyle::Overlay:
				resultStyle = overlayFrameStyle;
				break;

			case EWindowFrameStyle::Resizeable:
				resultStyle = resizeableFrameStyle;
				break;
		}

		return resultStyle;
	}

	Win32WindowGeometry _win32CheckWindowGeometry( const WindowGeometry & pWindowGeometry, EWindowSizeMode pSizeMode, HWND pWindowHwnd )
	{
	    RECT windowRect;
	    windowRect.left = 0;
	    windowRect.top = 0;
	    windowRect.right = static_cast<LONG>( pWindowGeometry.size.x );
	    windowRect.bottom = static_cast<LONG>( pWindowGeometry.size.y );

	    auto win32FrameStyle = _win32TranslateFrameStyle( pWindowGeometry.frameStyle );

	    if( ( win32FrameStyle == 0u ) && pWindowHwnd )
	    {
	        win32FrameStyle = ::GetWindowLongPtrA( pWindowHwnd, GWL_STYLE );
	    }

	    if( pSizeMode == EWindowSizeMode::ClientArea )
	    {
	        ::AdjustWindowRect( &windowRect, win32FrameStyle, FALSE );
	    }

	    Win32WindowGeometry resultGeometry;
	    resultGeometry.style = win32FrameStyle;
	    resultGeometry.frameRect.left = static_cast<int>( pWindowGeometry.position.x + windowRect.left );
	    resultGeometry.frameRect.top = static_cast<int>( pWindowGeometry.position.y + windowRect.top );
	    resultGeometry.frameRect.right = static_cast<int>( windowRect.right - windowRect.left );
	    resultGeometry.frameRect.bottom = static_cast<int>( windowRect.bottom - windowRect.top );

	    return resultGeometry;
	}

	LRESULT __stdcall _win32DefaultWindowEventCallback( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam )
	{
		return ::DefWindowProcA( pHWND, pMessage, pWparam, pLparam );
	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
