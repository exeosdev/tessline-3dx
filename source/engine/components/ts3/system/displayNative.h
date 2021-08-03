
#ifndef __TS3_SYSTEM_DISPLAY_NATIVE_H__
#define __TS3_SYSTEM_DISPLAY_NATIVE_H__

#include "displayCommon.h"
#include "coreSessionContextNative.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidDisplay.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32Display.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11Display.h"
#endif

namespace ts3
{
namespace system
{

    struct DsmDisplayManager
    {
        CoreSessionContextHandle csContext = nullptr;

        DsmDisplayManagerNativeData nativeData;
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_NATIVE_H__
