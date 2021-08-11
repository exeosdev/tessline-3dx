
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__

#include "win32Common.h"

namespace ts3
{
namespace system
{

	struct SysContextNativeData
	{
	    HINSTANCE appExecModuleHandle;
	};

	struct SysContextNativeProxyCreateInfo
    {
	    HINSTANCE appExecModuleHandle;
    };

	using Win32SysContextNativeData = SysContextNativeData;
	using Win32SysContextNativeProxyCreateInfo = SysContextNativeProxyCreateInfo;

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__
