
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__

#include "x11EventCore.h"

namespace ts3
{

    struct SysWindowNativeData
    {
        Display * display = nullptr;
        Window xWindow = XID_None;
        Colormap xColormap = XID_None;
    };

    struct SysX11WindowCreateInfo
    {
        Display * display = nullptr;
        int screenIndex = -1;
        Window rootWindow = XID_None;
        int colorDepth = 0;
        Visual * windowVisual = nullptr;
        Atom wmpDeleteWindow = -1;
        bool fullscreenMode = false;
    };

    using SysX11WindowNativeData = SysWindowNativeData;

	void sysX11CreateWindow( SysX11WindowNativeData & pWindowNativeData, const SysX11WindowCreateInfo & pCreateInfo );
	void sysX11UpdateNewWindowState( SysX11WindowNativeData & pWindowNativeData, const SysX11WindowCreateInfo & pCreateInfo );
	void sysX11DestroyWindow( SysX11WindowNativeData & pWindowNativeData );

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
