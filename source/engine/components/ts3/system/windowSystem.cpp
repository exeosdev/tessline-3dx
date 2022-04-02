
#include "displaySystem.h"
#include "windowSystem.h"

namespace ts3::system
{

    WindowManager::WindowManager( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( std::move( pDisplayManager ) )
    {}

    WindowManager::~WindowManager() noexcept = default;

    WindowHandle WindowManager::createWindow( WindowCreateInfo pCreateInfo )
    {
        auto createInfo = std::move( pCreateInfo );

        createInfo.frameGeometry = validateFrameGeometry( createInfo.frameGeometry );

        if( createInfo.title.empty() )
        {
            createInfo.title = "Tessline-3DX / Native Window";
        }

        return _nativeCreateWindow( std::move( createInfo ) );
    }

    bool WindowManager::checkFrameGeometry( const FrameGeometry & pFrameGeometry ) const
    {
        return mDisplayManager->checkFrameGeometry( pFrameGeometry );
    }

    FrameGeometry WindowManager::validateFrameGeometry( const FrameGeometry & pFrameGeometry ) const
    {
        return mDisplayManager->validateFrameGeometry( pFrameGeometry );
    }

    void WindowManager::onWindowDestroy( Window & pWindow ) noexcept
    {}


    Window::Window( WindowManagerHandle pWindowManager, void * pNativeData )
    : Frame( pWindowManager->mSysContext )
    , mWindowManager( std::move( pWindowManager ) )
    {
        setEventSourceNativeData( pNativeData, {} );
    }

    Window::~Window() noexcept
    {
        resetEventSourceNativeData();
    }

    void Window::resize( const FrameSize & pSize )
    {
        FrameGeometry newFrameGeometry{};
        newFrameGeometry.position = CX_FRAME_POS_AUTO;
        newFrameGeometry.size = pSize;
        newFrameGeometry.style = EFrameStyle::Unspecified;

        newFrameGeometry = mWindowManager->validateFrameGeometry( newFrameGeometry );

        const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_OUTER_RECT_BIT;
        _nativeUpdateGeometry( newFrameGeometry, updateFlags );
    }

    void Window::resizeClientArea( const FrameSize & pSize )
    {
        FrameGeometry newFrameGeometry{};
        newFrameGeometry.position = CX_FRAME_POS_AUTO;
        newFrameGeometry.size = pSize;
        newFrameGeometry.style = EFrameStyle::Unspecified;

        newFrameGeometry = mWindowManager->validateFrameGeometry( newFrameGeometry );

        const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_CLIENT_AREA_BIT;
        _nativeUpdateGeometry( newFrameGeometry, updateFlags );
    }

    void Window::setFullscreenMode( bool pEnable )
    {
        _nativeSetFullscreenMode( pEnable );
    }

    void Window::setTitle( const std::string & pTitleText )
    {
        _nativeSetTitle( pTitleText );
    }

    void Window::updateGeometry( const FrameGeometry & pFrameGeometry,
                                 Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
    {
        auto newFrameGeometry = mWindowManager->validateFrameGeometry( pFrameGeometry );
        _nativeUpdateGeometry( newFrameGeometry, pUpdateFlags );
    }

    FrameSize Window::getClientAreaSize() const
    {
        return _nativeGetSize( EFrameSizeMode::ClientArea );
    }

    FrameSize Window::getSize() const
    {
        return _nativeGetSize( EFrameSizeMode::ClientArea );
    }

    bool Window::isFullscreen() const
    {
        return false;
    }

} // namespace ts3::system
