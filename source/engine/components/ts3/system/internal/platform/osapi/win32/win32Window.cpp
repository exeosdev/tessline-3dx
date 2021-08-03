
#include <ts3/system/windowtem.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3
{

	void _win32RegisterWndClass( WindowNativeData & pWindowNativeData );
	void _win32UnregisterWndClass( WindowNativeData & pWindowNativeData );
	DWORD _win32TranslateFrameStyle( WmWindowFrameStyle pStyle );
	LRESULT __stdcall _win32DefaultWindowEventCallback( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );


	void WmWindowManager::_sysInitialize()
	{}

	void WmWindowManager::_sysRelease() noexcept
	{}


	void Window::_sysInitialize( const WmWindowCreateInfo & pCreateInfo )
	{
		win32CreateWindow( mNativeData, pCreateInfo );
		::ShowWindow( mNativeData.hwnd, SW_SHOWNORMAL );
	}

	void Window::_sysRelease() noexcept
	{
		win32DestroyWindow( mNativeData );
	}

	void Window::_sysGetClientAreaSize( WmWindowSize & pClientAreaSize ) const
	{
		RECT clientRect;
		::GetClientRect( mNativeData.hwnd, &clientRect );

		pClientAreaSize.x = clientRect.right - clientRect.left;
		pClientAreaSize.y = clientRect.bottom - clientRect.top;
	}

	void Window::_sysGetFrameSize( WmWindowSize & pFrameSize ) const
	{
		RECT clientRect;
		::GetClientRect( mNativeData.hwnd, &clientRect );

		pFrameSize.x = clientRect.right - clientRect.left;
		pFrameSize.y = clientRect.bottom - clientRect.top;
	}


	void win32CreateWindow( WindowNativeData & pWindowNativeData, const WmWindowCreateInfo & pCreateInfo )
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

		HWND windowHandle = ::CreateWindowExA( WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW,
		                                       pWindowNativeData.wndClassName,
		                                       pCreateInfo.properties.title.c_str(),
		                                       win32FrameStyle,
		                                       framePosX,
		                                       framePosY,
		                                       frameWidth,
		                                       frameHeight,
		                                       nullptr,
		                                       nullptr,
		                                       pWindowNativeData.wndProcModuleHandle,
		                                       &pWindowNativeData );

		if ( windowHandle == nullptr )
		{
			throw 0;
		}

		UINT updateFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
		::SetWindowPos( windowHandle, nullptr, 0, 0, 0, 0, updateFlags );

		pWindowNativeData.hwnd = windowHandle;
	}

	void win32DestroyWindow( WindowNativeData & pWindowNativeData )
	{
		::DestroyWindow( pWindowNativeData.hwnd );
		pWindowNativeData.hwnd = nullptr;
		pWindowNativeData.wndClassID = 0;
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
			windowClass.cbClsExtra = 0;
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

			pWindowNativeData.wndClassID = wndClassID;
		}

		pWindowNativeData.wndClassName = wndClassName;
		pWindowNativeData.wndProcModuleHandle = wndProcModuleHandle;
	}

	void _win32UnregisterWndClass( WindowNativeData & pWindowNativeData )
	{
		::UnregisterClassA( pWindowNativeData.wndClassName, pWindowNativeData.wndProcModuleHandle );

		pWindowNativeData.wndClassID = 0;
		pWindowNativeData.wndClassName = nullptr;
		pWindowNativeData.wndProcModuleHandle = nullptr;
	}

	DWORD _win32TranslateFrameStyle( WmWindowFrameStyle pStyle )
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
			case WmWindowFrameStyle::Caption:
				resultStyle = captionFrameStyle;
				break;

			case WmWindowFrameStyle::Fixed:
				resultStyle = fixedFrameStyle;
				break;

			case WmWindowFrameStyle::Overlay:
				resultStyle = overlayFrameStyle;
				break;

			case WmWindowFrameStyle::Resizeable:
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

}
#endif
