
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_SYSTEM_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_SYSTEM_CONTEXT_H__

#include "androidCommon.h"

namespace ts3
{

    struct SysContextNativeData
    {
        AndroidAppState * androidAppState = nullptr;
        ANativeWindow * androidNativeWindow = nullptr;
    };

	struct SysContextNativeCreateInfo
	{
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_SYSTEM_CONTEXT_H__
