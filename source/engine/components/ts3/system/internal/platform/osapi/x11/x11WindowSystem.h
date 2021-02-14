
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__

#include "x11EventCore.h"

namespace ts3
{

	struct SysWindowNativeData : public SysEventSourceNativeData
	{
		Colormap colormap = cvXIDNone;
	};

	struct X11SysWindowCreateInfo : public SysWindowCreateInfo
	{
		Display * display = nullptr;
		int screenIndex = -1;
		Window rootWindow = cvXIDNone;
		int colorDepth = 0;
		Visual * windowVisual = nullptr;
		Atom wmpDeleteWindow = -1;
		bool fullscreenMode = false;
	};

	void sysX11CreateWindow( SysWindowNativeData & pWindowNativeData, const X11SysWindowCreateInfo & pCreateInfo );
	void sysX11UpdateNewWindowState( SysWindowNativeData & pWindowNativeData, const X11SysWindowCreateInfo & pCreateInfo );
	void sysX11DestroyWindow( SysWindowNativeData & pWindowNativeData );

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_H__
