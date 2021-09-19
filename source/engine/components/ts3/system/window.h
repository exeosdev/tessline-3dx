
#ifndef __TS3_SYSTEM_WINDOW_H__
#define __TS3_SYSTEM_WINDOW_H__

#include "windowCommon.h"
#include "sysObject.h"

namespace ts3::system
{

    struct WindowManagerNativeData;
    struct WindowNativeData;

    ts3SysDeclareHandle( DisplayManager );
    ts3SysDeclareHandle( WindowManager );

    using WindowPredicateCallback = std::function<bool( Window & )>;

	struct WindowCreateInfo
	{
		WindowProperties properties;
	};

	class WindowManager : public SysObject
    {
    public:
        struct ObjectPrivateData;
        DisplayManagerHandle const mDisplayManager;
        std::unique_ptr<ObjectPrivateData> const mPrivate;
        const WindowManagerNativeData * const mNativeData = nullptr;

    public:
        explicit WindowManager( DisplayManagerHandle pDisplayManager );
        virtual ~WindowManager();

        TS3_PCL_ATTR_NO_DISCARD Window * createWindow( const WindowCreateInfo & pCreateInfo );

        void destroyWindow( Window & pWindow );

        void removeWindow( Window & pWindow );

        void reset();

        TS3_SYSTEM_API_NODISCARD Window * findWindow( WindowPredicateCallback pPredicate );

        TS3_SYSTEM_API_NODISCARD bool isWindowValid( Window & pWindow ) const;

        TS3_SYSTEM_API_NODISCARD bool checkWindowGeometry( const WindowGeometry & pWindowGeometry ) const;

        TS3_SYSTEM_API_NODISCARD WindowGeometry validateWindowGeometry( const WindowGeometry & pWindowGeometry ) const;

    private:
        void _nativeCtor();
        void _nativeDtor() noexcept;
        void _nativeCreateWindow( Window & pWindow, const WindowCreateInfo & pCreateInfo );
        void _nativeDestroyWindow( Window & pWindow );
        void _nativeRemoveWindow( Window & pWindow );
    };

	class Window : public SysObject
    {
    public:
        struct ObjectPrivateData;
        WindowManager * const mWindowManager;
        std::unique_ptr<ObjectPrivateData> const mPrivate;
        const WindowNativeData * const mNativeData = nullptr;

    public:
        explicit Window( WindowManager * pWindowManager );
        virtual ~Window();

        void destroy();

        void setFullscreenMode( bool pEnable = true, const WindowSize & pScreenSize = cvWindowSizeAuto );

        void resizeClientArea( const WindowSize & pNewWindowSize );

        void resizeFrame( const WindowSize & pNewWindowSize );

        void setTitleText( const std::string & pTitleText );

        void updateGeometry( const WindowGeometry & pWindowGeometry, WindowSizeMode pSizeMode );

        void syncInternalState();

        TS3_SYSTEM_API_NODISCARD WindowSize getClientAreaSize() const;

        TS3_SYSTEM_API_NODISCARD WindowSize getFrameSize() const;

        TS3_SYSTEM_API_NODISCARD bool isFullscreen() const;

    private:
        void _nativeSetTitleText( const std::string & pTitleText );
        void _nativeUpdateGeometry( const WindowGeometry & pWindowGeometry, WindowSizeMode pSizeMode );
        void _nativeGetSize( WindowSizeMode pSizeMode, WindowSize & pOutSize ) const;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_WINDOW_H__
