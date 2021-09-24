
#ifndef __TS3_SYSTEM_WINDOW_NATIVE_H__
#define __TS3_SYSTEM_WINDOW_NATIVE_H__

#include "window.h"
#include <list>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidWindow.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32Window.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11Window.h"
#endif

namespace ts3::system
{

    class EventController;

    enum EWindowStateFlags : uint32
    {
        E_WINDOW_STATE_FLAG_FULLSCREEN_BIT = 0x8000
    };

    struct WindowManager::ObjectInternalData
    {
        WindowManagerNativeData nativeDataPriv;
    };

    struct Window::ObjectInternalData
    {
        WindowNativeData nativeDataPriv;
        Bitmask<EWindowStateFlags> stateFlags = 0;
        WindowSize preFullscreenSavedSize;
    };

    TS3_SYSTEM_API void nativeBindEventSource( EventController & pEventController, Window & pWindow );

} // namespace ts3::system

#endif // __TS3_SYSTEM_WINDOW_NATIVE_H__
