
#include "windowNative.h"
#include "displayNative.h"
#include <ts3/stdext/utilities.h>

namespace ts3::system
{

    WindowManager::WindowManager( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( pDisplayManager )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {
        _nativeCtor();
    }

    WindowManager::~WindowManager()
    {
        _nativeDtor();
    }

    Window * WindowManager::createWindow( const WindowCreateInfo & pCreateInfo )
    {
        WindowCreateInfo validatedCreateInfo;
        validatedCreateInfo.properties = pCreateInfo.properties;
        validatedCreateInfo.properties.geometry = validateWindowGeometry( validatedCreateInfo.properties.geometry );

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

    Window * WindowManager::findWindow( WindowPredicateCallback pPredicate )
    {
        for( auto & window : mPrivate->windowList )
        {
            if( pPredicate( window ) )
            {
                return &window;
            }
        }
        return nullptr;
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
        const auto & windowPos = pWindowGeometry.position;
        const auto & windowSize = pWindowGeometry.size;

        auto screenSize = mDisplayManager->queryDefaultDisplaySize();
        auto minWindowSize = mDisplayManager->queryMinWindowSize();

        if( windowSize != cvWindowSizeMax )
        {
            if( ( windowSize.x == 0 ) || ( windowSize.y == 0 ) )
            {
                return false;
            }

            if( ( windowSize.x > screenSize.x ) || ( windowSize.y > screenSize.y ) )
            {
                return false;
            }

            if( ( windowSize.x < minWindowSize.x ) || ( windowSize.y < minWindowSize.y ) )
            {
                return false;
            }
        }

        if( windowPos != cvWindowPositionAuto )
        {
            if( ( windowPos.x < 0 ) || ( windowPos.y < 0 ) )
            {
                return false;
            }

            auto maxPosX = static_cast<int32>( screenSize.x - windowSize.x );
            auto maxPosY = static_cast<int32>( screenSize.y - windowSize.y );
            if ( ( windowPos.x > maxPosX ) || ( windowPos.y > maxPosY ) )
            {
                return false;
            }
        }

        return true;
    }

    WindowGeometry WindowManager::validateWindowGeometry( const WindowGeometry & pWindowGeometry ) const
    {
        const auto & windowPos = pWindowGeometry.position;
        const auto & windowSize = pWindowGeometry.size;

        auto resultGeometry = pWindowGeometry;

        auto screenSize = mDisplayManager->queryDefaultDisplaySize();
        auto minWindowSize = mDisplayManager->queryMinWindowSize();

        if( windowSize == cvWindowSizeMax )
        {
            // Window size exceeds the screen size - clamp the size.
            resultGeometry.size.x = screenSize.x;
            resultGeometry.size.y = screenSize.y;
        }
        else
        {
            if ( ( windowSize.x == 0 ) || ( windowSize.y == 0 ) )
            {
                // Any dimension set to 0 means "use default size". By default,
                // we just use the ratio of the screen and 70% of its dimensions.
                resultGeometry.size.x = static_cast<uint32>( screenSize.x * 0.7 );
                resultGeometry.size.y = static_cast<uint32>( screenSize.y * 0.7 );
            }
            else
            {
                // Size of the window must be less than the size of the screen...
                resultGeometry.size.x = getMinOf( resultGeometry.size.x, screenSize.x );
                resultGeometry.size.y = getMinOf( resultGeometry.size.y, screenSize.y );

                // ... but at the same time bigger than the minimum allowed size (Win32-specific, really).
                resultGeometry.size.x = getMaxOf( resultGeometry.size.x, minWindowSize.x );
                resultGeometry.size.y = getMaxOf( resultGeometry.size.y, minWindowSize.y );
            }
        }

        if ( ( windowPos.x < 0 ) || ( windowPos.y < 0 ) )
        {
            resultGeometry.position.x = static_cast<int32>( ( screenSize.x - windowSize.x ) / 2 );
            resultGeometry.position.y = static_cast<int32>( ( screenSize.y - windowSize.y ) / 2 );
        }
        else
        {
            auto maxPosX = static_cast<int32>( screenSize.x - windowSize.x );
            auto maxPosY = static_cast<int32>( screenSize.y - windowSize.y );
            resultGeometry.position.x = getMinOf( resultGeometry.position.x, maxPosX );
            resultGeometry.position.y = getMinOf( resultGeometry.position.y, maxPosY );
        }

        return resultGeometry;
    }


    Window::Window( WindowManager * pWindowManager )
    : SysObject( pWindowManager->mSysContext )
    , mWindowManager( pWindowManager )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    Window::~Window() = default;

    void Window::destroy()
    {
        mWindowManager->destroyWindow( *this );
    }

    void Window::setFullscreenMode( bool pEnable, const WindowSize & pScreenSize )
    {
        WindowGeometry newWindowGeometry;

        if( pEnable )
        {
            if( mPrivate->stateFlags.isSet( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT ) )
            {
                return;
            }

            auto screenSize = pScreenSize;
            if( screenSize == cvWindowSizeAuto )
            {
                screenSize = mWindowManager->mDisplayManager->queryDefaultDisplaySize();
            }

            newWindowGeometry.position = { 0, 0 };
            newWindowGeometry.size = screenSize;
            newWindowGeometry.frameStyle = WindowFrameStyle::Overlay;

            mPrivate->preFullscreenSavedSize = getFrameSize();
            mPrivate->stateFlags.set( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT );
        }
        else
        {
            if( !mPrivate->stateFlags.isSet( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT ) )
            {
                return;
            }

            newWindowGeometry.position = cvWindowPositionAuto;
            newWindowGeometry.size = mPrivate->preFullscreenSavedSize;
            newWindowGeometry.frameStyle = WindowFrameStyle::Caption;

            mPrivate->preFullscreenSavedSize = cvWindowSizeAuto;
            mPrivate->stateFlags.unset( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT );
        }

        newWindowGeometry = mWindowManager->validateWindowGeometry( newWindowGeometry );

        _nativeUpdateGeometry( newWindowGeometry, WindowSizeMode::FrameRect );
    }

    void Window::resizeClientArea( const WindowSize & pNewWindowSize )
    {
        WindowGeometry newWindowGeometry;
        newWindowGeometry.position = cvWindowPositionAuto;
        newWindowGeometry.size = pNewWindowSize;
        newWindowGeometry.frameStyle = WindowFrameStyle::Unspecified;

        newWindowGeometry = mWindowManager->validateWindowGeometry( newWindowGeometry );

        _nativeUpdateGeometry( newWindowGeometry, WindowSizeMode::ClientArea );
    }

    void Window::resizeFrame( const WindowSize & pNewWindowSize )
    {
        WindowGeometry newWindowGeometry;
        newWindowGeometry.position = cvWindowPositionAuto;
        newWindowGeometry.size = pNewWindowSize;
        newWindowGeometry.frameStyle = WindowFrameStyle::Unspecified;

        newWindowGeometry = mWindowManager->validateWindowGeometry( newWindowGeometry );

        _nativeUpdateGeometry( newWindowGeometry, WindowSizeMode::FrameRect );
    }

    void Window::setTitleText( const std::string & pTitleText )
    {
        _nativeSetTitleText( pTitleText );
    }

    void Window::updateGeometry( const WindowGeometry & pWindowGeometry, WindowSizeMode pSizeMode )
    {
        auto windowGeometry = mWindowManager->validateWindowGeometry( pWindowGeometry );
        _nativeUpdateGeometry( windowGeometry, pSizeMode );
    }

    void Window::syncInternalState()
    {}

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

    bool Window::isFullscreen() const
    {
        return mPrivate->stateFlags.isSet( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT );
    }

} // namespace ts3::system
