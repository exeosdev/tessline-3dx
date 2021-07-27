
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_DEFS_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_DEFS_H__

#include <ts3/platform/android/androidNativeAppProxy.h>

namespace ts3
{

    enum : uint32
    {
        SYS_ANDROID_APP_STATE_USER_DATA_INDEX_SYSTEM_CONTEXT,
        SYS_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER
    };

    struct AndroidSysContextNativeData
    {
        // Window associated with the main app's activity. It is provided by the system and
        // we do not own it, so, except some rare cases, (window updates, state transitions)
        // it should be always here. May be altered by the system when activity changes.
        ANativeWindow * androidNativeWindow = nullptr;
    };

    struct AndroidSysPlatformNativeDataDeleter
    {
    };

    using SysPlatformNativeData = AndroidSysPlatformNativeData;
    using SysPlatformNativeDataDeleter = AndroidSysPlatformNativeDataDeleter;

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_DEFS_H__
