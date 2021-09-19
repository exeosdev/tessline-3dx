
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_H__

#include "x11Common.h"
#include <X11/extensions/Xrandr.h>
#include <ts3/stdext/version.h>
#include <unordered_map>

namespace ts3::system
{

    struct DisplayManagerNativeData : public X11NativeDataCommon
	{
        uint16 screenDepth = 0;
	};

    struct DisplayDriverNativeDataGeneric : public X11NativeDataCommon
    {
        uint16 screenDepth = 0;
        Version xrrVersion = cvVersionUnknown;
        XRRScreenResources * xrrScreenResources = nullptr;
        XRRMonitorInfo * xrrMonitorList = nullptr;
        int xrrMonitorsNum = 0;
        XRRMonitorInfo * xrrDefaultMonitorInfo = nullptr;
        std::unordered_map<RRMode, XRRModeInfo *> xrrModeInfoMap;
    };

    struct DisplayAdapterNativeDataGeneric : public X11NativeDataCommon
    {
    };

    struct DisplayOutputNativeDataGeneric : public X11NativeDataCommon
    {
        RROutput xrrOutputID = E_X11_XID_NONE;
        RRCrtc xrrCrtcID = E_X11_XID_NONE;
    };

    struct DisplayVideoModeNativeDataGeneric : public X11NativeDataCommon
    {
        RRMode xrrModeID = E_X11_XID_NONE;
        XRRModeInfo  * xrrModeInfo = nullptr;
    };

    bool nativeX11CheckColorFormatSupport( XDisplay pXDisplay, int pScreenIndex, ColorFormat pColorFormat );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_H__
