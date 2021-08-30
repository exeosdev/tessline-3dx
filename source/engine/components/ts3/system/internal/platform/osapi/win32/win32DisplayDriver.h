
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__

#include "win32Common.h"

namespace ts3::system
{

    struct DisplayManagerNativeData
	{
	};

    struct DisplayDriverNativeDataGeneric
    {
    };

	struct DisplayAdapterNativeDataGeneric
    {
	    DISPLAY_DEVICEA gdiDeviceInfo;
	    std::string adapterUUID;
	    std::string adapterName;
	    std::string displayDeviceID;
    };

	struct DisplayOutputNativeDataGeneric
    {
	    HMONITOR gdiOutputMonitor;
	    std::string displayDeviceID;
	    std::string outputID;
    };

	struct DisplayVideoModeNativeDataGeneric
    {
	    DEVMODEA gdiModeInfo;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__
