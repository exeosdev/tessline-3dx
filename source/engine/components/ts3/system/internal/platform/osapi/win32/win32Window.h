
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__

#include "win32EventSystem.h"

namespace ts3::system
{

    using native_window_event_id_t = UINT;

    struct WindowCreateInfo;
    struct WindowProperties;

    struct WindowNativeData : public EventSourceNativeData
	{
		ATOM wndClsID = 0;
		LPCSTR wndClsName = nullptr;
		HMODULE moduleHandle = nullptr;
	};

	struct WindowManagerNativeData
    {
    };

	struct Win32WindowGeometry
    {
	    RECT frameRect;
	    DWORD style;
    };

	void nativeWin32CreateWindow( WindowNativeData & pWindowNativeData, const WindowCreateInfo & pCreateInfo );
	void nativeWin32DestroyWindow( WindowNativeData & pWindowNativeData );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__
