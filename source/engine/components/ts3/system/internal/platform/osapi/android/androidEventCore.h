
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_SYSTEM_H__

namespace ts3
{

    using NativeEvent = struct ANativeEvent;

    constexpr int cvAndroidLooperWaitTimeoutImmediate = 0;
    constexpr int cvAndroidLooperWaitTimeoutInfinity = -1;

    enum class ANativeEventType : enum_default_value_t
    {
        AppCommand,
        Input,
        Unknown
    };

    enum : int32_t
    {
        APP_CMD_USER_DESTROY_REQUESTED = 0xFF00
    };

    struct ANativeEvent
    {
        ANativeEventType type;
        union
        {
            int32 eAppCommand;
            AInputEvent * eInputEvent;
        };
    };

	struct EventSourceNativeData
	{
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_SYSTEM_H__
