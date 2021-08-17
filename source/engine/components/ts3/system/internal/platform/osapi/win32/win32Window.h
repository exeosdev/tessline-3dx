
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__

#include <ts3/system/window.h>
#include "win32Common.h"

namespace ts3
{
namespace system
{

	struct WindowNativeData
	{
	    HWND hwnd;
		ATOM wndClsID = 0;
		LPCSTR wndClsName = nullptr;
		HMODULE moduleHandle = nullptr;
	};

	void win32CreateWindow( WindowNativeData & pWindowNativeData, const WindowCreateInfo & pCreateInfo );
	void win32DestroyWindow( WindowNativeData & pWindowNativeData );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__
