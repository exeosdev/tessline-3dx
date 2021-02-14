
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_DRIVER_H__

#include "x11DisplayCommon.h"
#include <unordered_map>

namespace ts3
{

	struct SysDisplayDriverNativeDataGeneric : public XRRCommonDriverData
	{
		using XRRDisplayModeInfoMap = std::unordered_map<RRMode, XRRModeInfo*>;
		XRRDisplayModeInfoMap xrrDisplayModeInfoMap;
	};

	struct SysDsmAdapterNativeDataGeneric
	{
	};

	struct SysDsmOutputNativeDataGeneric
	{
		RROutput xrrOutputID = cvXIDNone;
		RRCrtc xrrCrtcID = cvXIDNone;
	};

	struct SysDsmVideoModeNativeDataGeneric
	{
		RRMode xrrModeID = cvXIDNone;
		XRRModeInfo * xrrModeInfo = nullptr;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_DRIVER_H__
