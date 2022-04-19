
#include "androidWindowSystem.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

	AndroidWindowManager::AndroidWindowManager( DisplayManagerHandle pDisplayManager )
	: WindowManager( std::move( pDisplayManager ) )
	{}

	AndroidWindowManager::~AndroidWindowManager() noexcept = default;

	WindowHandle AndroidWindowManager::_nativeCreateWindow( WindowCreateInfo pCreateInfo )
	{
		return nullptr;
	}


	AndroidWindow::AndroidWindow( AndroidWindowManagerHandle pWindowManager )
	: Window( std::move( pWindowManager ), &mNativeData )
	{}

	AndroidWindow::~AndroidWindow() noexcept = default;

	void AndroidWindow::_nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode )
	{}

	void AndroidWindow::_nativeSetFullscreenMode( bool pEnable )
	{}

	void AndroidWindow::_nativeSetTitle( const std::string & pTitle )
	{}

	void AndroidWindow::_nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
	                                           Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
	{}

	FrameSize AndroidWindow::_nativeGetSize( EFrameSizeMode /* pSizeMode */ ) const
	{
		auto & aSessionData = platform::androidGetASessionData( *this );
		return platform::androidQueryNativeWindowSize( aSessionData.aNativeWindow );
	}


	namespace platform
	{

		math::Size2u androidQueryNativeWindowSize( ANativeWindow * pANativeWindow )
		{
			math::Size2u resultSize{};

			auto width = ANativeWindow_getWidth( pANativeWindow );
			auto height = ANativeWindow_getHeight( pANativeWindow );

			resultSize.x = trunc_numeric_cast<decltype( resultSize.x )>( width );
			resultSize.y = trunc_numeric_cast<decltype( resultSize.x )>( height );

			return resultSize;
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
