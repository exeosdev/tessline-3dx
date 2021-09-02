
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

        validateWindowGeometry( validatedCreateInfo.properties.geometry );

        auto windowRef = mPrivate->windowList.emplace( mPrivate->windowList.end(), this );
        windowRef->mPrivate->windowManagerListRef = windowRef;

        _nativeCreateWindow( *windowRef, validatedCreateInfo );

        return &( *windowRef );
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

    void WindowManager::reset()
    {
        for( auto & window : mPrivate->windowList )
        {
            _nativeDestroyWindow( window );
        }
        mPrivate->windowList.clear();
    }

    bool WindowManager::isWindowValid( Window & pWindow ) const
    {
        if( mPrivate->windowList.empty() )
        {
            return false;
        }

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

    bool WindowManager::checkWindowGeometry( const WindowGeometry & pWindowGeometry ) const
    {
        return mDisplayManager->checkWindowGeometry( pWindowGeometry.position, pWindowGeometry.size );
    }

    bool WindowManager::validateWindowGeometry( WindowGeometry & pWindowGeometry ) const
    {
        return mDisplayManager->validateWindowGeometry( pWindowGeometry.position, pWindowGeometry.size );
    }


    Window::Window( WindowManager * pWindowManager )
    : mWindowManager( pWindowManager )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    Window::~Window() = default;

    void Window::destroy()
    {
        mWindowManager->destroyWindow( *this );
    }

    void Window::resizeClientArea( const WindowSize & pNewWindowSize )
    {
        _nativeResize( pNewWindowSize, WindowSizeMode::ClientArea );
    }

    void Window::resizeFrame( const WindowSize & pNewWindowSize )
    {
        _nativeResize( pNewWindowSize, WindowSizeMode::FrameRect );
    }

    void Window::setTitleText( const std::string & pTitleText )
    {
        _nativeSetTitleText( pTitleText );
    }

    void Window::updateGeometry( const WindowGeometry & pWindowGeometry )
    {
        _nativeUpdateGeometry( pWindowGeometry );
    }

    WindowSize Window::getClientAreaSize() const
    {
        WindowSize result;
        _nativeGetSize( WindowSizeMode::ClientArea, result );
        return result;
    }

    WindowSize Window::getFrameSize() const
    {
        WindowSize result;
        _nativeGetSize( WindowSizeMode::FrameRect, result );
        return result;
    }

} // namespace ts3::system
