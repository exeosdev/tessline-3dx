
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_SYSTEM_H__

#include <ts3/system/displayConfiguration.h>
#include <ts3/system/displaySystem.h>
#include "win32Common.h"

namespace ts3::system
{

	ts3SysDeclareHandle( Win32DisplayManager );
	ts3SysDeclareHandle( Win32DisplayDriver );

	namespace platform
	{

		struct Win32DisplayDriverNativeData
		{
		};

		struct Win32DisplayAdapterNativeData
		{
			std::string deviceUUID;
			std::string deviceName;
			DISPLAY_DEVICEA gdiDeviceInfo;
			std::string displayDeviceID;
		};

		struct Win32DisplayOutputNativeData
		{
			HMONITOR gdiMonitorHandle;
			std::string displayDeviceName;
			std::string outputID;
		};

		struct Win32DisplayVideoModeNativeData
		{
			DEVMODEA gdiModeInfo;
		};

	}

	/// @brief
	using Win32DisplayAdapter = Win32NativeObject<DisplayAdapter, platform::Win32DisplayAdapterNativeData>;
	using Win32DisplayOutput = Win32NativeObject<DisplayOutput, platform::Win32DisplayOutputNativeData>;
	using Win32DisplayVideoMode = Win32NativeObject<DisplayVideoMode, platform::Win32DisplayVideoModeNativeData>;

	/// @brief
	class Win32DisplayManager : public DisplayManager
	{
	public:
		explicit Win32DisplayManager( SysContextHandle pSysContext );
		virtual ~Win32DisplayManager() noexcept;

	private:
		virtual DisplayDriverHandle _nativeCreateDisplayDriver() override final;

        virtual void _nativeQueryDefaultDisplayOffset( DisplayOffset & pOutOffset ) const override final;

		virtual void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const override final;

		virtual void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const override final;
	};

	/// @brief
	class Win32DisplayDriver : public Win32NativeObject<DisplayDriver, platform::Win32DisplayDriverNativeData>
	{
	public:
		explicit Win32DisplayDriver( DisplayManagerHandle pDisplayManager );
		virtual ~Win32DisplayDriver() noexcept;

	private:
		virtual void _nativeEnumDisplayDevices() override final;

		virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat ) override final;

		virtual EColorFormat _nativeQueryDefaultSystemColorFormat() const override final;

		// Returns a handle to an existing adapter with a specified UUID (DeviceKey).
		SysHandle<Win32DisplayAdapter> _findAdapterByUUID( const std::string & pUUID );

		// Returns a handle to an existing output of a specified adapter with a given output name (DeviceID);
		SysHandle<Win32DisplayOutput> _findAdapterOutputForDisplayDeviceName( DisplayAdapter & pAdapter, const char * pDeviceName );

		// Returns a handle to an existing output of an existing adapter with a given output name (DeviceID);
		SysHandle<Win32DisplayOutput> _findAnyOutputForDisplayDeviceName( const char * pDeviceName );

		//
		static BOOL CALLBACK _win32MonitorEnumProc( HMONITOR pMonitorHandle, HDC pHDC, LPRECT pMonitorRect, LPARAM pUserParam );
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_SYSTEM_H__
