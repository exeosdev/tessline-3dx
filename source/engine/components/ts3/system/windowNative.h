
#ifndef __TS3_SYSTEM_WINDOW_NATIVE_H__
#define __TS3_SYSTEM_WINDOW_NATIVE_H__

#include "windowCommon.h"
#include <ts3/math/vector.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidWindow.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32Window.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11Window.h"
#endif

namespace ts3
{
namespace system
{

    struct WmWindow
    {
    };

    struct WmWindowManager
    {
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_WINDOW_NATIVE_H__
