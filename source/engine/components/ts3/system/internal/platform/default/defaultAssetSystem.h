
#ifndef __TS3_SYSTEM_PLATFORM_DEFAULT_ASSET_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_DEFAULT_ASSET_SYSTEM_H__

#include <ts3/system/assetCommon.h>
#include <ts3/system/fileCommon.h>

#if( TS3_SYSTEM_USE_ASSET_SYSTEM_DEFAULT )
namespace ts3::system
{

    struct AssetNativeData
    {
        FileHandle fileHandle = nullptr;
        std::string filePath;
    };

    struct AssetDirectoryNativeData
    {
        std::string combinedDirPath;
    };

    struct AssetLoaderNativeData
    {
        FileManagerHandle fileManager = nullptr;
        std::string rootDir;
    };

    struct AssetLoaderCreateInfoNativeParams
    {
        FileManagerHandle fileManager = nullptr;
        std::string relativeAssetRootDir;
    };

} // namespace ts3::system
#endif // TS3_SYSTEM_USE_ASSET_SYSTEM_DEFAULT

#endif // __TS3_SYSTEM_PLATFORM_DEFAULT_ASSET_SYSTEM_H__
