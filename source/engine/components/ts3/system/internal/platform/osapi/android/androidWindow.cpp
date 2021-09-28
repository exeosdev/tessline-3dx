
#include <ts3/system/windowNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

	math::Size2u nativeAndroidQueryNativeWindowSize( ANativeWindow * pANativeWindow )
	{
		math::Size2u resultSize{};

		auto width = ANativeWindow_getWidth( pANativeWindow );
		auto height = ANativeWindow_getHeight( pANativeWindow );

		resultSize.x = trunc_numeric_cast<decltype( resultSize.x )>( width );
		resultSize.y = trunc_numeric_cast<decltype( resultSize.x )>( height );

		return resultSize;
	}

	void WindowManager::_nativeConstructor()
	{
		mInternal->nativeDataPriv.setSessionData( nativeAndroidGetASessionData( *mSysContext ) );
	}

	void WindowManager::_nativeDestructor() noexcept
	{
		mInternal->nativeDataPriv.resetSessionData();
	}

	void WindowManager::_nativeCreateWindow( Window & pWindow, const WindowCreateInfo & pCreateInfo )
	{
		auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );

		auto & windowNativeData = pWindow.mInternal->nativeDataPriv;
		windowNativeData.setSessionData( aSessionData );
	}

	void WindowManager::_nativeDestroyWindow( Window & pWindow )
	{
		auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
		auto & windowNativeData = pWindow.mInternal->nativeDataPriv;

		windowNativeData.resetSessionData();
	}

	bool WindowManager::_nativeIsWindowValid( const Window & pWindow ) const noexcept
	{
		auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
		return aSessionData.aNativeWindow != nullptr;
	}


	void Window::_nativeSetTitleText( const std::string & /* pTitleText */ )
	{
	}

	void Window::_nativeUpdateGeometry( const WindowGeometry & pWindowGeometry, WindowSizeMode /* pSizeMode */ )
	{
	}

	void Window::_nativeGetSize( WindowSizeMode /* pSizeMode */, WindowSize & pOutSize ) const
	{
		auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
		pOutSize = nativeAndroidQueryNativeWindowSize( aSessionData.aNativeWindow );
	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
