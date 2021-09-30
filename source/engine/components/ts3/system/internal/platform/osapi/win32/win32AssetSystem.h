
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_ASSET_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_ASSET_SYSTEM_H__

#include "win32Common.h"

namespace ts3::system
{

    struct AssetNativeData
    {
        HANDLE fileHandle = nullptr;
        std::string filePath;
    };

    struct AssetDirectoryNativeData
    {
        std::string combinedDirPath;
    };

    struct AssetLoaderNativeData
    {
        std::string rootDir;
    };

    struct AssetLoaderCreateInfoNativeParams
    {
        std::string relativeAssetRootDir;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_ASSET_SYSTEM_H__
