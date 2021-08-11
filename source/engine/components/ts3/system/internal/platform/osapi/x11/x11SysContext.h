
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__

#include "x11Common.h"

namespace ts3
{
namespace system
{

	struct SysContextNativeData
	{
	    struct SessionInfo
        {
	        int connectionNumber;
	        std::string vendorName;
	        std::string displayString;
        };

	    Display * display = nullptr;
	    Window rootWindow = XID_None;
	    int screenIndex = -1;
	    SessionInfo sessionInfo;
	    Atom wmpDeleteWindow = -1;
	};

	using X11SysContextNativeData = SysContextNativeData;
	using X11SessionInfo = SysContextNativeData::SessionInfo;

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__
