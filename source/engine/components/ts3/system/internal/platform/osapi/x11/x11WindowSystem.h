
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_SYSTEM_H__

#include "x11EventCore.h"
#include <ts3/system/windowSystem.h>

namespace ts3::system
{

    ts3SysDeclareHandle( X11DisplayManager );
    ts3SysDeclareHandle( X11Window );
    ts3SysDeclareHandle( X11WindowManager );

    namespace platform
    {

        struct X11WindowNativeData : public X11EventSourceNativeData
        {
            Colormap xColormap = E_X11_XID_NONE;
        };

        struct X11WindowCreateInfo : public WindowCreateInfo, public X11NativeDataCommon
        {
            Visual * windowVisual = nullptr;
            int colorDepth = 0;
            bool fullscreenMode = false;
        };

        void x11CreateWindow( X11WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo );
        
        void x11UpdateNewWindowState( X11WindowNativeData & pWindowNativeData, const X11WindowCreateInfo & pCreateInfo );
        
        void x11DestroyWindow( X11WindowNativeData & pWindowNativeData );

        void x11SetFrameTitle( const X11WindowNativeData & pWindowNativeData, const std::string & pTitle );

        void x11UpdateFrameGeometry( const X11WindowNativeData & pWindowNativeData,
                                     const FrameGeometry & pFrameGeometry,
                                     Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags );

        TS3_SYSTEM_API_NODISCARD FrameSize x11GetFrameSize( const X11WindowNativeData & pWindowNativeData,
                                                            EFrameSizeMode pSizeMode );

    }

    class X11WindowManager : public X11NativeObject<WindowManager, platform::X11NativeDataCommon>
    {
    public:
        explicit X11WindowManager( X11DisplayManagerHandle pDisplayManager );
        virtual ~X11WindowManager() noexcept;

    private:
        // @override WindowManager::_nativeCreateWindow
        virtual WindowHandle _nativeCreateWindow( WindowCreateInfo pCreateInfo ) override final;
    };

    class X11Window : public X11NativeObject<Window, platform::X11WindowNativeData>
    {
        friend class X11WindowManager;

    public:
        explicit X11Window( X11WindowManagerHandle pWindowManager );
        virtual ~X11Window() noexcept;

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

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_WINDOW_SYSTEM_H__
