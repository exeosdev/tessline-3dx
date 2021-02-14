
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_CORE_H__

namespace ts3
{

    using SysNativeEvent = struct ANativeEvent;

    inline constexpr int cvAndroidLooperWaitTimeoutImmediate = 0;
    inline constexpr int cvAndroidLooperWaitTimeoutInfinity = -1;

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

	struct SysEventSourceNativeData
	{
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_CORE_H__
