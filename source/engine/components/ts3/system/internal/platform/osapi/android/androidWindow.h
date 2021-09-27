
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__

#include "androidEventSystem.h"

namespace ts3::system
{

	struct WindowManagerNativeData : public AndroidNativeDataCommon
	{
	};

	struct WindowNativeData : public EventSourceNativeData
	{
		ANativeWindow * aNativeWindow = nullptr;
	};

	TS3_SYSTEM_API_NODISCARD math::Size2u nativeAndroidQueryNativeWindowSize( ANativeWindow * pANativeWindow );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__
