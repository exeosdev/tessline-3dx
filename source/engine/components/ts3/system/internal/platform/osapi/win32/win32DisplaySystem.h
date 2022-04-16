
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
	class Win32DisplayAdapter : public Win32NativeObject<DisplayAdapter, platform::Win32DisplayAdapterNativeData>
	{
		friend class Win32DisplayDriver;

	public:
		explicit Win32DisplayAdapter( Win32DisplayDriver & pDisplayDriver );
		virtual ~Win32DisplayAdapter() noexcept;
	};

	/// @brief
	class Win32DisplayOutput : public Win32NativeObject<DisplayOutput, platform::Win32DisplayOutputNativeData>
	{
		friend class Win32DisplayDriver;

	public:
		explicit Win32DisplayOutput( Win32DisplayAdapter & pDisplayAdapter );
		virtual ~Win32DisplayOutput() noexcept;
	};

	/// @brief
	class Win32DisplayVideoMode : public Win32NativeObject<DisplayVideoMode, platform::Win32DisplayVideoModeNativeData>
	{
		friend class Win32DisplayDriver;

	public:
		explicit Win32DisplayVideoMode( Win32DisplayOutput & pDisplayOutput );
		virtual ~Win32DisplayVideoMode() noexcept;
	};

	/// @brief
	class Win32DisplayManager : public DisplayManager
	{
	public:
		explicit Win32DisplayManager( SysContextHandle pSysContext );
		virtual ~Win32DisplayManager() noexcept;

	private:
		virtual DisplayDriverHandle _nativeCreateDisplayDriver() override final;

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
		Handle<Win32DisplayAdapter> _findAdapterByUUID( const std::string & pUUID );

		// Returns a handle to an existing output of a specified adapter with a given output name (DeviceID);
		Handle<Win32DisplayOutput> _findAdapterOutputForDisplayDeviceName( DisplayAdapter & pAdapter, const char * pDeviceName );

		// Returns a handle to an existing output of an existing adapter with a given output name (DeviceID);
		Handle<Win32DisplayOutput> _findAnyOutputForDisplayDeviceName( const char * pDeviceName );

		//
		static BOOL CALLBACK _win32MonitorEnumProc( HMONITOR pMonitorHandle, HDC pHDC, LPRECT pMonitorRect, LPARAM pUserParam );
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_SYSTEM_H__
