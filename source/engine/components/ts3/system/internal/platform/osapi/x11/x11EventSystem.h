
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_SYSTEM_H__

#include "x11Common.h"

namespace ts3::system
{

    using NativeEvent = XEvent;

	enum class EX11MouseButtonID : enum_default_value_t
	{
		Unknown,
		Left,
		Middle,
		Right,
		VWheelUp,
		VWheelDown,
		HWheelLeft,
		HWheelRight,
		XBT1,
		XBT2
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_SYSTEM_H__
