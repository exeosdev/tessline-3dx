
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_WINDOW_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_WINDOW_SYSTEM_H__

#include "nsWindowProxy.h"
#include <ts3/system/windowSystem.h>
#include <AppKit/NSScreen.h>
#include <AppKit/NSWindow.h>

namespace ts3::system
{

    ts3SysDeclareHandle( OSXDisplayManager );
    ts3SysDeclareHandle( OSXWindow );
    ts3SysDeclareHandle( OSXWindowManager );

    namespace platform
    {

        struct OSXWindowNativeData
        {
            OSXWindow * parentWindow = nullptr;

            NSWindow __strong * nsWindow = nullptr;

            NSView __strong * nsWindowView = nullptr;

            NSScreen __strong * nsTargetScreen = nullptr;
        };

        void osxCreateWindow( OSXWindowNativeData & pWindowNativeData, const WindowCreateInfo & pCreateInfo );

        TS3_SYSTEM_API_NODISCARD NSUInteger osxTranslateFrameStyle( EFrameStyle pStyle );

    }

    class OSXWindowManager : public WindowManager
    {
    public:
        explicit OSXWindowManager( OSXDisplayManagerHandle pDisplayManager );
        virtual ~OSXWindowManager() noexcept;

    private:
        // @override WindowManager::_nativeCreateWindow
        virtual WindowHandle _nativeCreateWindow( WindowCreateInfo pCreateInfo ) override final;
    };

    class OSXWindow : public OSXNativeObject<Window, platform::OSXWindowNativeData>
    {
        friend class OSXWindowManager;

    public:
        explicit OSXWindow( OSXWindowManagerHandle pWindowManager );
        virtual ~OSXWindow() noexcept;

    private:
        // @override Window::_nativeResize
        virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) override final;

        // @override Window::_nativeSetFullscreenMode
        virtual void _nativeSetFullscreenMode( bool pEnable ) override final;

        // @override Window::_nativeSetTitle
        virtual void _nativeSetTitle( const std::string & pTitle ) override final;

        // @override Window::_nativeUpdateGeometry
        virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
                                            Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

        // @override Window::_nativeGetSize
        virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_WINDOW_SYSTEM_H__
