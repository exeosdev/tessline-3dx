
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__

#include "x11CommonDefs.h"

namespace ts3
{

	enum class ESysX11MouseButtonID : enum_default_value_t
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

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
