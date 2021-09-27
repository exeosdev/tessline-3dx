
#ifndef __TS3_SYSTEM_WINDOW_H__
#define __TS3_SYSTEM_WINDOW_H__

#include "windowCommon.h"
#include "eventSystem.h"
#include "sysObject.h"

namespace ts3::system
{

    struct WindowManagerNativeData;
    struct WindowNativeData;

    ts3SysDeclareHandle( DisplayManager );

    using WindowPredicateCallback = std::function<bool( Window & )>;

	struct WindowCreateInfo
	{
		WindowProperties properties;
	};

	class Window : public EventSource
    {
		friend class WindowManager;
    public:
        struct ObjectInternalData;
        WindowManager * const mWindowManager;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const WindowNativeData * const mNativeData = nullptr;

    public:
        explicit Window( WindowManager * pWindowManager );
        virtual ~Window() noexcept;

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

	class WindowManager : public SysObject
    {
	    friend class Window;

    public:
        struct ObjectInternalData;
        DisplayManagerHandle const mDisplayManager;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const WindowManagerNativeData * const mNativeData = nullptr;

    public:
        explicit WindowManager( DisplayManagerHandle pDisplayManager );
        virtual ~WindowManager() noexcept;

        TS3_PCL_ATTR_NO_DISCARD WindowHandle createWindow( const WindowCreateInfo & pCreateInfo );

        TS3_PCL_ATTR_NO_DISCARD bool isWindowValid( Window & pWindow ) const;

        TS3_PCL_ATTR_NO_DISCARD bool checkWindowGeometry( const WindowGeometry & pWindowGeometry ) const;

        TS3_PCL_ATTR_NO_DISCARD WindowGeometry validateWindowGeometry( const WindowGeometry & pWindowGeometry ) const;

    friendapi:
        // Used by the Window class. It is called inside its destructor.
        void onWindowDestroy( Window & pWindow ) noexcept;

    private:
        void _nativeConstructor();
        void _nativeDestructor() noexcept;
        void _nativeCreateWindow( Window & pWindow, const WindowCreateInfo & pCreateInfo );
        void _nativeDestroyWindow( Window & pWindow );
        bool _nativeIsWindowValid( const Window & pWindow ) const noexcept;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_WINDOW_H__
