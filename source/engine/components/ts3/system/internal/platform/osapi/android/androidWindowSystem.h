
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__

#include "androidEventCore.h"
#include <ts3/system/windowSystem.h>

namespace ts3::system
{

	ts3SysDeclareHandle( AndroidWindow );
	ts3SysDeclareHandle( AndroidWindowManager );

	namespace platform
	{

		using AndroidWindowManagerNativeData = AndroidNativeDataCommon;
		using AndroidWindowNativeData = AndroidNativeDataCommon;

		TS3_SYSTEM_API_NODISCARD math::Size2u androidQueryNativeWindowSize( ANativeWindow * pANativeWindow );

	}

	class AndroidWindowManager : public WindowManager, public NativeObject<platform::AndroidWindowManagerNativeData>
	{
	public:
		explicit AndroidWindowManager( DisplayManagerHandle pDisplayManager );
		virtual ~AndroidWindowManager() noexcept;

	private:
		// @override WindowManager::_nativeCreateWindow
		virtual WindowHandle _nativeCreateWindow( WindowCreateInfo pCreateInfo ) override final;
	};

	class AndroidWindow : public Window, public NativeObject<platform::AndroidWindowNativeData>
	{
		friend class AndroidWindowManager;

	public:
		explicit AndroidWindow( AndroidWindowManagerHandle pWindowManager );
		virtual ~AndroidWindow() noexcept;

	private:
		// @override Window::_nativeResize
		virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) override final;

		// @override Window::_nativeSetFullscreenMode
		virtual void _nativeSetFullscreenMode( bool pEnable ) override final;

		// @override Window::_nativeSetTitle
		virtual void _nativeSetTitle( const std::string & pTitle ) override final;

		// @override Window::_nativeUpdateGeometry
		virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
											Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

		// @override Window::_nativeGetSize
		virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const override final;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_WINDOW_H__
