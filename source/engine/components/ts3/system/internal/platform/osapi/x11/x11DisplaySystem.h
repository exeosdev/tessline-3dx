
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_SYSTEM_H__

#include "x11Common.h"
#include <ts3/system/displayConfiguration.h>
#include <ts3/system/displaySystem.h>
#include <X11/extensions/Xrandr.h>
#include <ts3/stdext/version.h>
#include <unordered_map>

namespace ts3::system
{

    ts3SysDeclareHandle( X11DisplayManager );
    ts3SysDeclareHandle( X11DisplayDriver );

    namespace platform
    {

        struct X11DisplayManagerNativeData : public X11NativeDataCommon
        {
            uint32 screenDepth = 0;
        };

        struct X11DisplayDriverNativeData : public X11NativeDataCommon
        {
            uint32 screenDepth = 0;
            Version xrrVersion = CX_VERSION_UNKNOWN;
            XRRScreenResources * xrrScreenResources = nullptr;
            XRRMonitorInfo * xrrMonitorList = nullptr;
            int xrrMonitorsNum = 0;
            XRRMonitorInfo * xrrDefaultMonitorInfo = nullptr;
            std::unordered_map<RRMode, XRRModeInfo *> xrrModeInfoMap;
        };

        struct X11DisplayAdapterNativeData : public X11NativeDataCommon
        {
        };

        struct X11DisplayOutputNativeData : public X11NativeDataCommon
        {
            RROutput xrrOutputID = E_X11_XID_NONE;
            RRCrtc xrrCrtcID = E_X11_XID_NONE;
        };

        struct X11DisplayVideoModeNativeData : public X11NativeDataCommon
        {
            RRMode xrrModeID = E_X11_XID_NONE;
            XRRModeInfo * xrrModeInfo = nullptr;
        };

    }

    using X11DisplayAdapter = X11NativeObject<DisplayAdapter, platform::X11DisplayAdapterNativeData>;
    using X11DisplayOutput = X11NativeObject<DisplayOutput, platform::X11DisplayOutputNativeData>;
    using X11DisplayVideoMode = X11NativeObject<DisplayVideoMode, platform::X11DisplayVideoModeNativeData>;

    /// @brief Implementation of DisplayManager for the X11 subsystem.
    class X11DisplayManager : public X11NativeObject<DisplayManager, platform::X11DisplayManagerNativeData>
    {
    public:
        explicit X11DisplayManager( SysContextHandle pSysContext );
        virtual ~X11DisplayManager() noexcept;

    private:
        void _initializeX11DisplayManagerState();
        void _releaseX11DisplayManagerState();

        virtual DisplayDriverHandle _nativeCreateDisplayDriver() override final;

        virtual void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const override final;

        virtual void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const override final;
    };

    /// @brief Implementation of DisplayDriver for the X11 subsystem.
    class X11DisplayDriver : public X11NativeObject<DisplayDriver, platform::X11DisplayDriverNativeData>
    {
    public:
        explicit X11DisplayDriver( X11DisplayManagerHandle pDisplayManager );
        virtual ~X11DisplayDriver() noexcept;

    private:
        void _initializeX11DisplayDriverState();
        void _releaseX11DisplayDriverState();

        virtual void _nativeEnumDisplayDevices() override final;

        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat ) override final;

        virtual EColorFormat _nativeQueryDefaultSystemColorFormat() const override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_DISPLAY_SYSTEM_H__
