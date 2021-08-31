
#include "windowNative.h"
#include "displayManager.h"
#include <ts3/stdext/utilities.h>

namespace ts3::system
{

    WindowManager::WindowManager( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( pDisplayManager )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    WindowManager::~WindowManager() = default;

    Window * WindowManager::createWindow( const WindowCreateInfo & pCreateInfo )
    {
        WindowCreateInfo validatedCreateInfo;
        validatedCreateInfo.properties = pCreateInfo.properties;

        validateWindowGeometryUpdate( validatedCreateInfo.properties.geometry );

        auto windowRef = mPrivate->windowList.emplace( mPrivate->windowList.end(), this );
        windowRef->mPrivate->windowManagerListRef = windowRef;

        _nativeCreateWindow( *windowRef, validatedCreateInfo );
    }

    void WindowManager::destroyWindow( Window & pWindow )
    {
        if( !isWindowValid( pWindow ) )
        {
            throw 0;
        }

        _nativeDestroyWindow( pWindow );

        mPrivate->windowList.erase( pWindow.mPrivate->windowManagerListRef );
    }

    void WindowManager::removeWindow( Window & pWindow )
    {
        if( !isWindowValid( pWindow ) )
        {
            throw 0;
        }

        _nativeRemoveWindow( pWindow );

        mPrivate->windowList.erase( pWindow.mPrivate->windowManagerListRef );
    }

    bool WindowManager::isWindowValid( Window & pWindow ) const
    {
        auto windowIter = mPrivate->windowList.begin();
        auto windowEndIter = mPrivate->windowList.end();

        while( windowIter != windowEndIter )
        {
            if( pWindow.mPrivate->windowManagerListRef == windowIter )
            {
                if( &( *windowIter ) == &pWindow )
                {
                    return true;
                }
            }
            ++windowIter;
        }

        return false;
    }

    bool WindowManager::validateWindowGeometry( const WindowGeometry & pWindowGeometry ) const
    {
        return true;
    }

    bool WindowManager::validateWindowGeometryUpdate( WindowGeometry & pWindowGeometry ) const
    {
        return true;
    }

} // namespace ts3::system
