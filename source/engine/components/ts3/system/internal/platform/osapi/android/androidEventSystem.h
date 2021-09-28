
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_SYSTEM_H__

#include "androidCommon.h"

namespace ts3::system
{

    using NativeEvent = struct ANativeEvent;

    enum class ANativeEventType : enum_default_value_t
    {
        AppCommand,
        Input,
        Unknown
    };

    enum : int32_t
    {
        E_ANDROID_EVT_CMD_USER_DESTROY_REQUESTED = 0xFF00,
    };

    inline constexpr int32_t CX_ANDROID_EVENT_LOOPER_WAIT_TIMEOUT_IMMEDIATE = 0;
    inline constexpr int32_t CX_ANDROID_EVENT_LOOPER_WAIT_TIMEOUT_INFINITY = -1;

    struct ANativeEvent
    {
        ANativeEventType type;
        union
        {
            int32 eAppCommand;
            AInputEvent * eInputEvent;
        };
    };

	struct EventSourceNativeData : public AndroidNativeDataCommon
	{
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_SYSTEM_H__
