
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_DEFS_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_DEFS_H__

#include <ts3/platform/android/androidNativeAppProxy.h>

namespace ts3
{

    enum : uint32
    {
        SYS_ANDROID_APP_STATE_USER_DATA_INDEX_CORE_SESSION_CONTEXT,
        SYS_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER
    };

    struct AndroidContextNativeData
    {
        // Window associated with the main app's activity. It is provided by the system and
        // we do not own it, so, except some rare cases, (window updates, state transitions)
        // it should be always here. May be altered by the system when activity changes.
        ANativeWindow * androidNativeWindow = nullptr;
    };

    struct AndroidPlatformNativeDataDeleter
    {
    };

    using PlatformNativeData = AndroidPlatformNativeData;
    using PlatformNativeDataDeleter = AndroidPlatformNativeDataDeleter;

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_DEFS_H__
