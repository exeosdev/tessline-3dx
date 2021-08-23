
#include "windowNative.h"
#include "displayNative.h"
#include <ts3/stdext/utilities.h>

namespace ts3
{
namespace system
{

    void _nativeCreateWindow( Window & pWindow, const WindowCreateInfo & pCreateInfo );
    void _nativeWindowGetClientAreaSize( Window & pWindow, WindowSize & pOutSize );
    void _nativeWindowGetFrameSize( Window & pWindow, WindowSize & pOutSize );
    
    WindowManagerHandle wmCreateWindowManager( DisplayManagerHandle pDisplayManager )
    {
        auto * windowManager = new WindowManager();
        windowManager->displayManager = pDisplayManager;

        return windowManager;
    }

    void wmDestroyWindowManager( WindowManagerHandle pWindowManager )
    {
        pWindowManager->displayManager = nullptr;
        delete pWindowManager;
    }

    WindowHandle wmCreateWindow( WindowManagerHandle pWindowManager,
                                   const WindowCreateInfo & pCreateInfo )
   {
        WindowCreateInfo validatedCreateInfo;
        validatedCreateInfo.properties = pCreateInfo.properties;

        wmWindowManagerValidateWindowGeometry( pWindowManager, validatedCreateInfo.properties.geometry );

        auto * window = new Window();
        window->windowManager = pWindowManager;

        _nativeCreateWindow( *window, validatedCreateInfo );

        return window;
   }

    WindowSize wmWindowGetClientAreaSize( WindowHandle pWindow )
    {
        WindowSize result;
        _nativeWindowGetClientAreaSize( *pWindow, result );
        return result;
    }

    WindowSize wmWindowGetFrameSize( WindowHandle pWindow )
    {
        WindowSize result;
        _nativeWindowGetFrameSize( *pWindow, result );
        return result;
    }

    bool wmWindowManagerValidateWindowGeometry( WindowManagerHandle pWindowManager,
                                                const WindowGeometry & pWindowGeometry )
    {
        return dsmDisplayManagerValidateWindowGeometry( pWindowManager->displayManager,
                                                        pWindowGeometry.position,
                                                        pWindowGeometry.size );
    }

    bool wmWindowManagerValidateWindowGeometry( WindowManagerHandle pWindowManager,
                                                WindowGeometry & pWindowGeometry )
    {
        return dsmDisplayManagerValidateWindowGeometry( pWindowManager->displayManager,
                                                        pWindowGeometry.position,
                                                        pWindowGeometry.size );
    }

} // namespace system
} // namespace ts3
