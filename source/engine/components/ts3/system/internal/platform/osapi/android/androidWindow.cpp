
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

		ANativeWindow_acquire( aSessionData.aNativeWindow );

		auto & windowNativeData = pWindow.mInternal->nativeDataPriv;
		windowNativeData.setSessionData( aSessionData );
		windowNativeData.aNativeWindow = aSessionData.aNativeWindow;
	}

	void WindowManager::_nativeDestroyWindow( Window & pWindow )
	{
		auto & windowNativeData = pWindow.mInternal->nativeDataPriv;

		ANativeWindow_release( windowNativeData.aNativeWindow );

		windowNativeData.aNativeWindow = nullptr;
		windowNativeData.resetSessionData();
	}

	bool WindowManager::_nativeIsWindowValid( const Window & pWindow ) const noexcept
	{
		auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
		auto & windowNativeData = pWindow.mInternal->nativeDataPriv;

		return windowNativeData.aNativeWindow && ( windowNativeData.aNativeWindow == aSessionData.aNativeWindow );
	}


	void Window::_nativeSetTitleText( const std::string & /* pTitleText */ )
	{
	}

	void Window::_nativeUpdateGeometry( const WindowGeometry & pWindowGeometry, WindowSizeMode /* pSizeMode */ )
	{
	}

	void Window::_nativeGetSize( WindowSizeMode /* pSizeMode */, WindowSize & pOutSize ) const
	{
		pOutSize = nativeAndroidQueryNativeWindowSize( mInternal->nativeDataPriv.aNativeWindow );
	}

} // namespace ts3::system
#endif
