
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_MANAGER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_MANAGER_H__

#include "x11DisplayXRR.h"

namespace ts3
{
namespace system
{

	struct DisplayManagerNativeData : public XRRCommonDriverData
	{
		XRRMonitorInfo * xrrDefaultMonitorInfo = nullptr;
	};

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_MANAGER_H__
