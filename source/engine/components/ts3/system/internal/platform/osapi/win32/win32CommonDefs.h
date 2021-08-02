
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_DEFS_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_DEFS_H__

#include <comdef.h>
#include <windows.h>

namespace ts3
{

	template <typename Tp>
	using ComPtr = Microsoft::WRL::ComPtr<Tp>;

    struct Win32ContextNativeData
    {
        HINSTANCE appExecModuleHandle;
    };

    struct Win32PlatformNativeDataDeleter
    {
    };

    using PlatformNativeData = Win32PlatformNativeData;
    using PlatformNativeDataDeleter = Win32PlatformNativeDataDeleter;

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_DEFS_H__
