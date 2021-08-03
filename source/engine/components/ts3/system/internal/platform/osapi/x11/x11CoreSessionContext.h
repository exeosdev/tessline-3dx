
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__

#include "x11CommonDefs.h"

namespace ts3
{
namespace system
{

	struct CoreSessionContextNativeData
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

	using X11CoreSessionContextNativeData = CoreSessionContextNativeData;
	using X11SessionInfo = CoreSessionContextNativeData::SessionInfo;

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__
