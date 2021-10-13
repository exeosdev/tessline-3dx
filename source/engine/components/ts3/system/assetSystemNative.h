
#ifndef __TS3_SYSTEM_ASSET_SYSTEM_NATIVE_H__
#define __TS3_SYSTEM_ASSET_SYSTEM_NATIVE_H__

#include "assetCommon.h"

#if( TS3_SYSTEM_USE_ASSET_SYSTEM_DEFAULT )
#  include "internal/platform/common/fileBasedAssetSystem.h"
#else
#  if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#    include "internal/platform/osapi/android/androidAssetSystem.h"
#  endif
#endif

namespace ts3::system
{

    struct AssetLoaderCreateInfo
    {
        platform::AssetLoaderCreateInfoNativeParams nativeParams;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_ASSET_SYSTEM_NATIVE_H__
