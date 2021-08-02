
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__

#include "x11EventCore.h"

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

    struct X11WindowCreateInfo
    {
        Display * display = nullptr;
        int screenIndex = -1;
        Window rootWindow = XID_None;
        int colorDepth = 0;
        Visual * windowVisual = nullptr;
        Atom wmpDeleteWindow = -1;
        bool fullscreenMode = false;
    };

    using X11WindowNativeData = WindowNativeData;

	void sysX11CreateWindow( X11WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo );
	void sysX11UpdateNewWindowState( X11WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo );
	void sysX11DestroyWindow( X11WindowNativeData & pWindowNativeData );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
