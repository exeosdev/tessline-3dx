
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__

#include "x11CommonDefs.h"

namespace ts3
{
namespace system
{

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

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
