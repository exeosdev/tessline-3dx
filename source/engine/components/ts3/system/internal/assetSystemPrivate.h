
#ifndef __TS3_SYSTEM_ASSET_SYSTEM_PRIVATE_H__
#define __TS3_SYSTEM_ASSET_SYSTEM_PRIVATE_H__

#include <ts3/system/assetSystem.h>

namespace ts3::system
{

    struct Asset::AssetPrivateData
    {
        std::string name;
    };

    struct AssetDirectory::AssetDirectoryPrivateData
    {
        std::string dirName;
        AssetNameList assetNameList;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_ASSET_SYSTEM_PRIVATE_H__
