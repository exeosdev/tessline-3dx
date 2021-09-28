
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_DISPLAY_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_DISPLAY_H__

#include "androidCommon.h"

namespace ts3::system
{

    struct DisplayManagerNativeData : public AndroidNativeDataCommon
    {
    };

    struct DisplayDriverNativeDataGeneric : public AndroidNativeDataCommon
    {
    };

    struct DisplayAdapterNativeDataGeneric : public AndroidNativeDataCommon
    {
    };

    struct DisplayOutputNativeDataGeneric : public AndroidNativeDataCommon
    {
    };

    struct DisplayVideoModeNativeDataGeneric : public AndroidNativeDataCommon
    {
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_DISPLAY_H__
