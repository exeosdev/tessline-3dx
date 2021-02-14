
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__

#include "win32Common.h"
#include <windowsx.h> // For GET_X_LPARAM/GET_Y_LPARAM

namespace ts3
{

	using SysNativeEvent = MSG;

	struct SysEventSourceNativeData
	{
		// Handle of the control which is the event emitter.
		// System event emitter is strictly designed for windows and GL surfaces.
		HWND hwnd = nullptr;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__
