
#ifndef __TS3_SYSTEM_WINDOW_SYSTEM_H__
#define __TS3_SYSTEM_WINDOW_SYSTEM_H__

#include "frame.h"
#include "windowCommon.h"

namespace ts3::system
{

    ts3DeclareClassHandle( DisplayManager );
    ts3DeclareClassHandle( WindowManager );

    struct WindowCreateInfo : public WindowProperties
    {
    };

    /// @brief
    class WindowManager : public SysObject
    {
        friend class Window;

    public:
        DisplayManagerHandle const mDisplayManager;

    public:
        explicit WindowManager( DisplayManagerHandle pDisplayManager );
        virtual ~WindowManager() noexcept;

        TS3_FUNC_NO_DISCARD WindowHandle createWindow( WindowCreateInfo pCreateInfo );

        TS3_FUNC_NO_DISCARD bool checkFrameGeometry( const FrameGeometry & pFrameGeometry ) const;

        TS3_FUNC_NO_DISCARD FrameGeometry validateFrameGeometry( const FrameGeometry & pFrameGeometry ) const;

    friendapi:
        // Used by the Window class. It is called inside its destructor.
        virtual void onWindowDestroy( Window & pWindow ) noexcept;

    private:
        virtual WindowHandle _nativeCreateWindow( WindowCreateInfo pCreateInfo ) = 0;
    };

    /// @brief
    class Window : public Frame
    {
        friend class WindowManager;

    public:
        WindowManagerHandle const mWindowManager;

    public:
        virtual ~Window() noexcept;

        virtual void resize( const FrameSize & pSize ) override final;

        virtual void resizeClientArea( const FrameSize & pSize ) override final;

        virtual void setFullscreenMode( bool pEnable ) override final;

        virtual void setTitle( const std::string & pTitleText ) override final;

        virtual void updateGeometry( const FrameGeometry & pFrameGeometry,
                                     Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

        TS3_FUNC_NO_DISCARD virtual FrameSize getClientAreaSize() const override final;

        TS3_FUNC_NO_DISCARD virtual FrameSize getSize() const override final;

        TS3_FUNC_NO_DISCARD virtual bool isFullscreen() const override final;

    protected:
        explicit Window( WindowManagerHandle pWindowManager, void * pNativeData );

    private:
        virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) = 0;

        virtual void _nativeSetFullscreenMode( bool pEnable ) = 0;

        virtual void _nativeSetTitle( const std::string & pTitle ) = 0;

        virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
                                            Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) = 0;

        virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const = 0;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_WINDOW_SYSTEM_H__
