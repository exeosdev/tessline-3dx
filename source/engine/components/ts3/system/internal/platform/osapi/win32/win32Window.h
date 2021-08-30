
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__

#include "win32Common.h"

namespace ts3::system
{

    struct WindowCreateInfo;
    struct WindowProperties;

	struct WindowNativeData
	{
	    HWND hwnd;
		ATOM wndClsID = 0;
		LPCSTR wndClsName = nullptr;
		HMODULE moduleHandle = nullptr;
	};

	void nativeWin32CreateWindow( WindowNativeData & pWindowNativeData, const WindowCreateInfo & pCreateInfo );
	void nativeWin32DestroyWindow( WindowNativeData & pWindowNativeData );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__
