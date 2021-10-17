
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__

#include "androidEventCore.h"

namespace ts3::system
{

	struct WindowManagerNativeData : public AndroidNativeDataCommon
	{
	};

	struct WindowNativeData : public EventSourceNativeData
	{
	};

	TS3_SYSTEM_API_NODISCARD math::Size2u nativeAndroidQueryNativeWindowSize( ANativeWindow * pANativeWindow );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__
