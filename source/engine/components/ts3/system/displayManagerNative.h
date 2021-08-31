
#ifndef __TS3_SYSTEM_DISPLAY_MANAGER_NATIVE_H__
#define __TS3_SYSTEM_DISPLAY_MANAGER_NATIVE_H__

#include "displayManager.h"
#include "sysContextNative.h"
#include <functional>
#include <map>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidDisplayManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32DisplayManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11DisplayManager.h"
#endif

namespace ts3::system
{

    using DisplayDriverFactoryCallback = std::function<DisplayDriverHandle()>;
    using DisplayDriverFactoryMap = std::map<EDisplayDriverType, DisplayDriverFactoryCallback>;

    struct DisplayManager::ObjectPrivateData
    {
        DisplayManagerNativeData nativeDataPriv;
        DisplayDriverFactoryMap driverFactoryMap;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_MANAGER_NATIVE_H__
