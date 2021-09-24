
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__

#include "x11EventSystem.h"
#include <ts3/system/windowCommon.h>

namespace ts3::system
{

    struct WindowManagerNativeData : public X11NativeDataCommon
    {
    };

    struct WindowNativeData : public EventSourceNativeData
    {
        Colormap xColormap = E_X11_XID_NONE;
    };

    struct X11WindowCreateInfo : public X11NativeDataCommon
    {
        WindowProperties commonProperties;
        Visual * windowVisual = nullptr;
        int colorDepth = 0;
        bool fullscreenMode = false;
    };

    void nativeX11CreateWindow( WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo );
    void nativeX11UpdateNewWindowState( WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo );
    void nativeX11DestroyWindow( WindowNativeData & pWindowNativeData );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
