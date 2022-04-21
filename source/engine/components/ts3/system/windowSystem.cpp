
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

	void WindowManager::releaseSystemWindow( Window & pWindow ) noexcept
	{
		try
		{
			_nativeDestroyWindow( pWindow );
		}
		catch( const Exception & pException )
		{
			( pException );
			ts3DebugInterrupt();
		}
		catch( ... )
		{
			ts3DebugInterrupt();
		}
	}


	Window::Window( WindowManagerHandle pWindowManager, void * pNativeData )
	: Frame( pWindowManager->mSysContext )
	, mWindowManager( std::move( pWindowManager ) )
	{
		setEventSourceNativeData( pNativeData );
	}

	Window::~Window() noexcept
	{
		resetEventSourceNativeData();
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

	void Window::resizeFrame( const FrameSize & pSize )
	{
		FrameGeometry newFrameGeometry{};
		newFrameGeometry.position = CX_FRAME_POS_AUTO;
		newFrameGeometry.size = pSize;
		newFrameGeometry.style = EFrameStyle::Unspecified;

		newFrameGeometry = mWindowManager->validateFrameGeometry( newFrameGeometry );

		const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_OUTER_RECT_BIT;
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

	FrameSize Window::getFrameSize() const
	{
		return _nativeGetSize( EFrameSizeMode::OuterRect );
	}

	bool Window::isFullscreen() const
	{
		return false;
	}

	void Window::onDestroySystemObjectRequested()
	{
		EventSource::onDestroySystemObjectRequested();

		// Window itself must be destroyed at the very end - it can still be an active event source!
		// Destroying it first would make it impossible to handle the event source removal (e.g. in
		// case of Win32, HWND would be already invalid inside EventSource::onDestroySystemObjectRequested()).

		mWindowManager->releaseSystemWindow( *this );
	}

} // namespace ts3::system
