
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__

#include "win32EventCore.h"

namespace ts3
{

	struct Win32WindowNativeData
	{
		ATOM wndClassID = 0;
		LPCSTR wndClassName = nullptr;
		HMODULE wndProcModuleHandle = nullptr;
	};

	void win32CreateWindow( WindowNativeData & pWindowNativeData, const WmWindowCreateInfo & pCreateInfo );
	void win32DestroyWindow( WindowNativeData & pWindowNativeData );

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_WINDOW_H__
