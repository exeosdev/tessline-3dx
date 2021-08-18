
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__

#include "androidCommon.h"

namespace ts3
{

    struct ContextNativeData
    {
        AndroidAppState * androidAppState = nullptr;
        ANativeWindow * androidNativeWindow = nullptr;
    };

	struct ContextNativeCreateInfo
	{
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__