
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_COMMON_H__

#include "x11Common.h"
#include <X11/extensions/Xrandr.h>

namespace ts3
{

	struct XRRCommonDriverData
	{
		Version xrrVersion = cvVersionUnknown;
		Display * display = nullptr;
		Window rootWindow = cvXIDNone;
		int screenIndex = -1;
		XRRScreenResources * xrrScreenResources = nullptr;
		XRRMonitorInfo * xrrMonitorList = nullptr;
		int xrrMonitorsNum = 0;
	};

	void x11XRRInitializeDriverState( XRRCommonDriverData * pXRRDriverData );
	void x11XRRReleaseDriverState( XRRCommonDriverData * pXRRDriverData );
	void x11XRREnumScreenResources( XRRCommonDriverData * pXRRDriverData );
	void x11XRRFreeScreenResources( XRRCommonDriverData * pXRRDriverData );
	void x11XRRResetDriverState( XRRCommonDriverData * pXRRDriverData );

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_COMMON_H__
