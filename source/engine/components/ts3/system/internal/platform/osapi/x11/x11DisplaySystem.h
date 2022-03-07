
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
            XRRModeInfo  * xrrModeInfo = nullptr;
        };

        TS3_FUNC_NO_DISCARD bool x11CheckColorFormatSupport( XDisplay pXDisplay, int pScreenIndex, EColorFormat pColorFormat );

    }

    /// @brief
    class X11DisplayAdapter : public X11NativeObject<DisplayAdapter, platform::X11DisplayAdapterNativeData>
    {
        friend class X11DisplayDriver;

    public:
        explicit X11DisplayAdapter( X11DisplayDriver & pDisplayDriver );
        virtual ~X11DisplayAdapter() noexcept;
    };

    /// @brief
    class X11DisplayOutput : public X11NativeObject<DisplayOutput, platform::X11DisplayOutputNativeData>
    {
        friend class X11DisplayDriver;

    public:
        explicit X11DisplayOutput( X11DisplayAdapter & pDisplayAdapter );
        virtual ~X11DisplayOutput() noexcept;
    };

    /// @brief
    class X11DisplayVideoMode : public X11NativeObject<DisplayVideoMode, platform::X11DisplayVideoModeNativeData>
    {
        friend class X11DisplayDriver;

    public:
        explicit X11DisplayVideoMode( X11DisplayOutput & pDisplayOutput );
        virtual ~X11DisplayVideoMode() noexcept;
    };

    /// @brief
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

    /// @brief
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
