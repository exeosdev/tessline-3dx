
#include "windowNative.h"
#include "displayNative.h"

namespace ts3::system
{

    Window::Window( WindowManager * pWindowManager )
    : EventSource( pWindowManager->mSysContext )
    , mWindowManager( pWindowManager )
    , mInternal( std::make_unique<ObjectInternalData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {
        setEventSourceNativeData( &( mInternal->nativeDataPriv ) );
    }

    Window::~Window() noexcept
    {
        mWindowManager->onWindowDestroy( *this );
    }

    void Window::setFullscreenMode( bool pEnable, const WindowSize & pScreenSize )
    {
        WindowGeometry newWindowGeometry;

        if( pEnable )
        {
            if( mInternal->stateFlags.isSet( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT ) )
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

            mInternal->preFullscreenSavedSize = getFrameSize();
            mInternal->stateFlags.set( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT );
        }
        else
        {
            if( !mInternal->stateFlags.isSet( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT ) )
            {
                return;
            }

            newWindowGeometry.position = cvWindowPositionAuto;
            newWindowGeometry.size = mInternal->preFullscreenSavedSize;
            newWindowGeometry.frameStyle = WindowFrameStyle::Caption;

            mInternal->preFullscreenSavedSize = cvWindowSizeAuto;
            mInternal->stateFlags.unset( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT );
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
        return mInternal->stateFlags.isSet( E_WINDOW_STATE_FLAG_FULLSCREEN_BIT );
    }


    WindowManager::WindowManager( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( pDisplayManager )
    , mInternal( std::make_unique<ObjectInternalData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {
        _nativeConstructor();
    }

    WindowManager::~WindowManager() noexcept
    {
        _nativeDestructor();
    }

    WindowHandle WindowManager::createWindow( const WindowCreateInfo & pCreateInfo )
    {
        WindowCreateInfo validatedCreateInfo;
        validatedCreateInfo.properties = pCreateInfo.properties;
        validatedCreateInfo.properties.geometry = validateWindowGeometry( validatedCreateInfo.properties.geometry );

        auto window = createSysObject<Window>( this );

        _nativeCreateWindow( *window, validatedCreateInfo );

        return window;
    }

    bool WindowManager::isWindowValid( Window & pWindow ) const
    {
        return _nativeIsWindowValid( pWindow );
    }

    bool WindowManager::checkWindowGeometry( const WindowGeometry & pWindowGeometry ) const
    {
        return mDisplayManager->checkWindowGeometry( pWindowGeometry );
    }

    WindowGeometry WindowManager::validateWindowGeometry( const WindowGeometry & pWindowGeometry ) const
    {
        return mDisplayManager->validateWindowGeometry( pWindowGeometry );
    }

    void WindowManager::onWindowDestroy( Window & pWindow ) noexcept
    {
        try
        {
            if( !isWindowValid( pWindow ) )
            {
                ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            pWindow.unregisterEventSourceAuto();

            _nativeDestroyWindow( pWindow );
        }
        catch( const Exception & pException )
        {
            ts3DebugInterrupt();
        }
        catch( ... )
        {
            ts3DebugInterrupt();
        }
    }

} // namespace ts3::system
