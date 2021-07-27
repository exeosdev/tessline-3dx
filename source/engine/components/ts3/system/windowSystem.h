
#ifndef __TS3_SYSTEM_WINDOW_H__
#define __TS3_SYSTEM_WINDOW_H__

#include "eventCore.h"
#include "windowDefs.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidWindow.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32Window.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11Window.h"
#endif

namespace ts3
{

	ts3DeclareSysHandle( SysDisplayManager );
	ts3DeclareSysHandle( SysWindow );
	ts3DeclareSysHandle( SysWindowManager );

	struct SysWindowCreateInfo;
	struct SysWindowNativeData;

	class SysDisplayManager;
	class SysEventController;

	struct SysWindowCreateInfo
	{
		SysWindowProperties properties;
	};

	class SysWindowManager : public SysBaseObject
	{
	public:
		SysDisplayManagerHandle const mDisplayManager;

		explicit SysWindowManager( SysDisplayManagerHandle pDisplayManager ) noexcept;
		virtual ~SysWindowManager() noexcept;

		static SysWindowManagerHandle create( SysDisplayManagerHandle pDisplayManager );

		SysWindowHandle createWindow( const SysWindowCreateInfo & pCreateInfo );

		bool validateWindowGeometry( SysWindowGeometry & pWindowGeometry ) const;

		TS3_PCL_ATTR_NO_DISCARD SysWindowSize queryDisplaySize() const;
		TS3_PCL_ATTR_NO_DISCARD SysWindowSize queryMinWindowSize() const;

	private:
		void _sysInitialize();
		void _sysRelease() noexcept;
	};

	class SysWindow : public SysBaseObject, public SysEventSource
	{
		friend class SysWindowManager;

	public:
		SysWindowNativeData mNativeData;
		SysWindowManagerHandle const mWindowManager;

		explicit SysWindow( SysWindowManagerHandle pWindowManager ) noexcept;
		virtual ~SysWindow() noexcept;

		static SysWindowHandle create( SysWindowManagerHandle pWindowManager, const SysWindowCreateInfo & pCreateInfo );

		TS3_PCL_ATTR_NO_DISCARD SysWindowSize getClientAreaSize() const;
		TS3_PCL_ATTR_NO_DISCARD SysWindowSize getFrameSize() const;

	private:
		void _sysInitialize( const SysWindowCreateInfo & pCreateInfo );
		void _sysRelease() noexcept;
		void _sysGetClientAreaSize( SysWindowSize & pClientAreaSize ) const;
		void _sysGetFrameSize( SysWindowSize & pFrameSize ) const;
	};

}

#endif // __TS3_SYSTEM_WINDOW_H__
