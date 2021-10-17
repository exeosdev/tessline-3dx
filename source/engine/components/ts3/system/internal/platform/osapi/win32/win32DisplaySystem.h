
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_SYSTEM_H__

#include <ts3/system/displayConfiguration.h>
#include <ts3/system/displaySystem.h>
#include "win32Common.h"

namespace ts3::system
{

    namespace platform
    {

        struct Win32DisplayDriverNativeDataGeneric
        {
        };

        struct Win32DisplayAdapterNativeDataGeneric
        {
            std::string deviceUUID;
            std::string deviceName;
            DISPLAY_DEVICEA gdiDeviceInfo;
            std::string displayDeviceID;
        };

        struct Win32DisplayOutputNativeDataGeneric
        {
            HMONITOR gdiMonitorHandle;
            std::string displayDeviceName;
            std::string outputID;
        };

        struct Win32DisplayVideoModeNativeDataGeneric
        {
            DEVMODEA gdiModeInfo;
        };

    }

    using Win32DisplayAdapterGeneric = NativeDisplayAdapter<platform::Win32DisplayAdapterNativeDataGeneric>;
    using Win32DisplayOutputGeneric = NativeDisplayOutput<platform::Win32DisplayOutputNativeDataGeneric>;
    using Win32DisplayVideoModeGeneric = NativeDisplayVideoMode<platform::Win32DisplayVideoModeNativeDataGeneric>;

    /// @brief
    class Win32DisplayManager : public DisplayManager
    {
    public:
        explicit Win32DisplayManager( SysContextHandle pSysContext );
        virtual ~Win32DisplayManager() noexcept;

    private:
        virtual DisplayDriverHandle _nativeCreateDisplayDriverGeneric() override final;

        virtual void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const override final;

        virtual void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const override final;
    };

    /// @brief
    class Win32DisplayDriverGeneric : public NativeDisplayDriver<platform::Win32DisplayDriverNativeDataGeneric>
    {
    public:
        explicit Win32DisplayDriverGeneric( DisplayManagerHandle pDisplayManager );
        virtual ~Win32DisplayDriverGeneric() noexcept;

    private:
        virtual void _nativeEnumDisplayDevices() override final;

        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat ) override final;

        virtual EColorFormat _nativeQueryDefaultSystemColorFormat() const override final;

        // Returns a handle to an existing adapter with a specified UUID (DeviceKey).
        Handle<Win32DisplayAdapterGeneric> _findAdapterByUUID( const std::string & pUUID );

        // Returns a handle to an existing output of a specified adapter with a given output name (DeviceID);
        Handle<Win32DisplayOutputGeneric> _findAdapterOutputForDisplayDeviceName( DisplayAdapter & pAdapter, const char * pDeviceName );

        // Returns a handle to an existing output of any of existing adapters with a given output name (DeviceID);
        Handle<Win32DisplayOutputGeneric> _findAnyOutputForDisplayDeviceName( const char * pDeviceName );

        //
        static BOOL CALLBACK _win32MonitorEnumProc( HMONITOR pMonitorHandle, HDC pHDC, LPRECT pMonitorRect, LPARAM pUserParam );
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_DISPLAY_SYSTEM_H__
