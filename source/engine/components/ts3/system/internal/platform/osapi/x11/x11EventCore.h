
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__

#include "x11Common.h"

namespace ts3
{

	using SysNativeEvent = XEvent;

	enum class SysX11MouseButtonID : enum_default_value_t
	{
		Unknown,
		Left,
		Middle,
		Right,
		VWheelUp,
		VWheelDown,
		HWheelLeft,
		HWheelRight,
		Xbtn1,
		Xbtn2
	};

	struct SysEventSourceNativeData
	{
		Display * display = nullptr;
		Window xwindow = cvXIDNone;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
