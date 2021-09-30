
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_ASSET_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_ASSET_SYSTEM_H__

#include "androidCommon.h"
#include <android/asset_manager.h>

namespace ts3::system
{

    struct AssetNativeData : public AndroidNativeDataCommon
    {
        AAssetManager * aAssetManager = nullptr;
        AAsset * aAsset = nullptr;
    };

    struct AssetDirectoryNativeData : public AndroidNativeDataCommon
    {
        AAssetManager * aAssetManager = nullptr;
        AAssetDir * aAssetDir = nullptr;
    };

    struct AssetLoaderNativeData : public AndroidNativeDataCommon
    {
        AAssetManager * aAssetManager = nullptr;
    };

    struct AssetLoaderCreateInfoNativeParams
    {
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_ASSET_SYSTEM_H__
