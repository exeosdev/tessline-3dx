
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_WINDOW_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_WINDOW_SYSTEM_H__

#include "osxEventCore.h"
#include <ts3/system/windowSystem.h>
#import <AppKit/NSScreen.h>
#import <AppKit/NSWindow.h>

namespace ts3::system
{

	ts3SysDeclareHandle( OSXDisplayManager );
	ts3SysDeclareHandle( OSXWindow );
	ts3SysDeclareHandle( OSXWindowManager );

	namespace platform
	{

		struct OSXWindowNativeData : public OSXEventSourceNativeData
		{
			OSXWindow * parentWindow = nullptr;

			NSScreen * nsTargetScreen = nullptr;
		};

		struct OSXFrameGeometry
		{
			NSRect frameRect;
			NSUInteger style;
		};

		void osxCreateWindow( OSXWindowNativeData & pWindowNativeData, NSScreen * pTargetScreen, const WindowCreateInfo & pCreateInfo );

		void osxCreateWindowDefaultView( OSXWindowNativeData & pWindowNativeData );

		void osxSetInputWindow( OSXWindowNativeData & pWindowNativeData );

		void osxSetFrameTitle( NSWindow * pNSWindow, const std::string & pTitle );

		void osxUpdateFrameGeometry( NSWindow * pNSWindow, const FrameGeometry & pFrameGeometry, Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags );

		TS3_SYSTEM_API_NODISCARD FrameSize osxGetFrameSize( NSWindow * pNSWindow, EFrameSizeMode pSizeMode );

		TS3_SYSTEM_API_NODISCARD NSUInteger osxTranslateFrameStyle( EFrameStyle pStyle );

	}

	class OSXWindowManager : public OSXNativeObject<WindowManager, platform::OSXNativeDataCommon>
	{
	public:
		explicit OSXWindowManager( OSXDisplayManagerHandle pDisplayManager );
		virtual ~OSXWindowManager() noexcept;

	private:
		// @override WindowManager::_nativeCreateWindow
		virtual WindowHandle _nativeCreateWindow( WindowCreateInfo pCreateInfo ) override final;

		// @override WindowManager::_nativeDestroyWindow
		virtual void _nativeDestroyWindow( Window & pWindow ) override final;
	};

	class OSXWindow : public OSXNativeObject<Window, platform::OSXWindowNativeData>
	{
		friend class OSXWindowManager;

	public:
		explicit OSXWindow( OSXWindowManagerHandle pWindowManager );
		virtual ~OSXWindow() noexcept;

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

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_WINDOW_SYSTEM_H__
