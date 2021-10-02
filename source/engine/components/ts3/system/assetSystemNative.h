
#ifndef __TS3_SYSTEM_ASSET_SYSTEM_NATIVE_H__
#define __TS3_SYSTEM_ASSET_SYSTEM_NATIVE_H__

#include "assetSystem.h"

#if( TS3_SYSTEM_USE_ASSET_SYSTEM_DEFAULT )
#  include "internal/platform/default/defaultAssetSystem.h"
#else
#  if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#    include "internal/platform/osapi/android/androidAssetSystem.h"
#  endif
#endif

namespace ts3::system
{

    struct AssetLoaderCreateInfo
    {
        SysContextHandle sysContext;
        AssetLoaderCreateInfoNativeParams nativeParams;
    };

    struct Asset::ObjectInternalData
    {
        AssetNativeData nativeDataPriv;
        std::string name;
    };

    struct AssetDirectory::ObjectInternalData
    {
        AssetDirectoryNativeData nativeDataPriv;
        std::string dirName;
        AssetNameList assetNameList;
    };

    struct AssetLoader::ObjectInternalData
    {
        AssetLoaderNativeData nativeDataPriv;
    };

    TS3_SYSTEM_API AssetLoaderHandle createAssetLoader( AssetLoaderCreateInfo pCreateInfo );


} // namespace ts3::system

#endif // __TS3_SYSTEM_ASSET_SYSTEM_NATIVE_H__
