
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__

#include "win32Common.h"
#include <string>

namespace ts3
{

	struct SysDisplayDriverNativeDataGeneric
	{
	};

	struct SysDsmAdapterNativeDataGeneric
	{
		DISPLAY_DEVICEA gdiDeviceInfo;
		std::string adapterUUID;
        std::string adapterName;
        std::string displayDeviceID;
	};

	struct SysDsmOutputNativeDataGeneric
	{
		HMONITOR gdiOutputMonitor;
        std::string displayDeviceID;
		std::string outputID;
	};

	struct SysDsmVideoModeNativeDataGeneric
	{
		DEVMODEA gdiModeInfo;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_DRIVER_H__
