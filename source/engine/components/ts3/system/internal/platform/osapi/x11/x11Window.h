
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__

#include "x11CommonDefs.h"

namespace ts3
{
namespace system
{

    struct WindowNativeData
    {
        Display * display = nullptr;
        Window xWindow = XID_None;
        Colormap xColormap = XID_None;
    };

    struct WindowManagerNativeData
    {
        Display * display = nullptr;
        Window xWindow = XID_None;
        Colormap xColormap = XID_None;
    };

    struct X11WmWindowCreateInfo
    {
        Display * display = nullptr;
        int screenIndex = -1;
        Window rootWindow = XID_None;
        int colorDepth = 0;
        Visual * windowVisual = nullptr;
        Atom wmpDeleteWindow = -1;
        bool fullscreenMode = false;
    };

    using X11WmWindowNativeData = WindowNativeData;
    using X11WmWindowManagerNativeData = WindowManagerNativeData;

    void x11CreateWindow( X11WmWindowNativeData & pWindowNativeData, const X11WmWindowCreateInfo & pCreateInfo );
    void x11UpdateNewWindowState( X11WmWindowNativeData & pWindowNativeData, const X11WmWindowCreateInfo & pCreateInfo );
    void x11DestroyWindow( X11WmWindowNativeData & pWindowNativeData );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
