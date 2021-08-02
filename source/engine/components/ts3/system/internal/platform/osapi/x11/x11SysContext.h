
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_SYSTEM_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_SYSTEM_CONTEXT_H__

#include "x11Common.h"

namespace ts3
{
namespace system
{

	struct X11SessionExtInfo
	{
		int connectionNumber;
		std::string vendorName;
		std::string displayString;
	};

	struct ContextNativeData
	{
		Display * display = nullptr;
		Window rootWindow = cvXIDNone;
		int screenIndex = -1;
		Atom wmpDeleteWindow = -1;
		X11SessionExtInfo sessionExtInfo;
	};

	struct ContextNativeCreateInfo
	{
	};

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_SYSTEM_CONTEXT_H__
