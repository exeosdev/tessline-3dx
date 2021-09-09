
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__

#include "win32Common.h"

namespace ts3::system
{

    struct DisplayDriverNativeDataGeneric
    {
    };

	struct DisplayAdapterNativeDataGeneric
    {
	    std::string deviceUUID;
	    std::string deviceName;

	    DISPLAY_DEVICEA gdiDeviceInfo;
	    std::string displayDeviceID;
    };

	struct DisplayOutputNativeDataGeneric
    {
	    HMONITOR gdiMonitorHandle;
	    std::string displayDeviceName;
	    std::string outputID;
    };

	struct DisplayVideoModeNativeDataGeneric
    {
	    DEVMODEA gdiModeInfo;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__
