
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_MANAGER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_MANAGER_H__

#include "x11DisplayCommon.h"

namespace ts3
{

	struct SysDisplayManagerNativeData : public XRRCommonDriverData
	{
		XRRMonitorInfo * xrrDefaultMonitorInfo = nullptr;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_MANAGER_H__
