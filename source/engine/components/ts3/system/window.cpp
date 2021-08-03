
#include "windowNative.h"
#include "displayNative.h"
#include <ts3/stdext/utilities.h>

namespace ts3
{
namespace system
{

    void _nativeWmCreateWindow( WmWindow & pWindow, const WmWindowCreateInfo & pCreateInfo );
    void _nativeWmWindowGetClientAreaSize( WmWindow & pWindow, WmWindowSize & pOutSize );
    void _nativeWmWindowGetFrameSize( WmWindow & pWindow, WmWindowSize & pOutSize );
    
    WmWindowManagerHandle wmCreateWindowManager( DsmDisplayManagerHandle pDisplayManager )
    {
        auto * windowManager = new WmWindowManager();
        windowManager->displayManager = pDisplayManager;

        return windowManager;
    }

    void wmDestroyWindowManager( WmWindowManagerHandle pWindowManager )
    {
        pWindowManager->displayManager = nullptr;
        delete pWindowManager;
    }

    WmWindowHandle wmCreateWindow( WmWindowManagerHandle pWindowManager,
                                   const WmWindowCreateInfo & pCreateInfo )
   {
        WmWindowCreateInfo validatedCreateInfo;
        validatedCreateInfo.properties = pCreateInfo.properties;

        wmWindowManagerValidateWindowGeometry( pWindowManager, validatedCreateInfo.properties.geometry );

        auto * window = new WmWindow();
        window->windowManager = pWindowManager;

        _nativeWmCreateWindow( *window, validatedCreateInfo );

        return window;
   }

    WmWindowSize wmWindowGetClientAreaSize( WmWindowHandle pWindow )
    {
        WmWindowSize result;
        _nativeWmWindowGetClientAreaSize( *pWindow, result );
        return result;
    }

    WmWindowSize wmWindowGetFrameSize( WmWindowHandle pWindow )
    {
        WmWindowSize result;
        _nativeWmWindowGetFrameSize( *pWindow, result );
        return result;
    }

    bool wmWindowManagerValidateWindowGeometry( WmWindowManagerHandle pWindowManager,
                                                const WmWindowGeometry & pWindowGeometry )
    {
        return dsmDisplayManagerValidateWindowGeometry( pWindowManager->displayManager,
                                                        pWindowGeometry.position,
                                                        pWindowGeometry.size );
    }

    bool wmWindowManagerValidateWindowGeometry( WmWindowManagerHandle pWindowManager,
                                                WmWindowGeometry & pWindowGeometry )
    {
        return dsmDisplayManagerValidateWindowGeometry( pWindowManager->displayManager,
                                                        pWindowGeometry.position,
                                                        pWindowGeometry.size );
    }

} // namespace system
} // namespace ts3
