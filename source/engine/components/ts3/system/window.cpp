
#include "windowNative.h"
#include "displayNative.h"
#include <ts3/stdext/utilities.h>

namespace ts3
{
namespace system
{

    void _nativeWmCreateWindow( Window & pWindow, const WindowCreateInfo & pCreateInfo );
    void _nativeWmWindowGetClientAreaSize( Window & pWindow, WindowSize & pOutSize );
    void _nativeWmWindowGetFrameSize( Window & pWindow, WindowSize & pOutSize );
    
    WindowManagerHandle wmCreateWindowManager( DsmDisplayManagerHandle pDisplayManager )
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

        _nativeWmCreateWindow( *window, validatedCreateInfo );

        return window;
   }

    WindowSize wmWindowGetClientAreaSize( WindowHandle pWindow )
    {
        WindowSize result;
        _nativeWmWindowGetClientAreaSize( *pWindow, result );
        return result;
    }

    WindowSize wmWindowGetFrameSize( WindowHandle pWindow )
    {
        WindowSize result;
        _nativeWmWindowGetFrameSize( *pWindow, result );
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
