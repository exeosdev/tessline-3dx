
#ifndef __TS3_SYSTEM_ASSET_COMMON_H__
#define __TS3_SYSTEM_ASSET_COMMON_H__

#include "prerequisites.h"

#if( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_ANDROID )
#  define TS3_SYSTEM_USE_ASSET_SYSTEM_DEFAULT 0
#else
#  define TS3_SYSTEM_USE_ASSET_SYSTEM_DEFAULT 1
#endif

namespace ts3::system
{

    class Asset;
    class AssetDirectory;
    class AssetLoader;

} // namespace ts3::system

#endif // __TS3_SYSTEM_ASSET_COMMON_H__
