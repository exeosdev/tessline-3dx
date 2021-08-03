
#ifndef __TS3_SYSTEM_WINDOW_H__
#define __TS3_SYSTEM_WINDOW_H__

#include "eventCore.h"
#include "windowDefs.h"

namespace ts3
{
namespace system
{

	ts3DeclareHandle( DsmDisplayManager );
	ts3DeclareHandle( Window );
	ts3DeclareHandle( WindowManager );

	struct WindowCreateInfo;
	struct WindowNativeData;

	class DsmDisplayManager;
	class EventController;

	struct WindowCreateInfo
	{
		WindowProperties properties;
	};

	class WindowManager : public BaseObject
	{
	public:
		DisplayManagerHandle const mDisplayManager;

		explicit WindowManager( DisplayManagerHandle pDisplayManager ) noexcept;
		virtual ~WindowManager() noexcept;

		static WindowManagerHandle create( DisplayManagerHandle pDisplayManager );

		WindowHandle createWindow( const WindowCreateInfo & pCreateInfo );

		bool validateWindowGeometry( WindowGeometry & pWindowGeometry ) const;

		TS3_PCL_ATTR_NO_DISCARD WindowSize queryDisplaySize() const;
		TS3_PCL_ATTR_NO_DISCARD WindowSize queryMinWindowSize() const;

	private:
		void _sysInitialize();
		void _sysRelease() noexcept;
	};


	TS3_SYSTEM_API WindowM

	TS3_SYSTEM_API WindowHandle sysCreateWindow( WindowManagerHandle pWindowManager, const WindowCreateInfo & pCreateInfo );

	class Window : public BaseObject, public EventSource
	{
		friend class WindowManager;

	public:
		WindowNativeData mNativeData;
		WindowManagerHandle const mWindowManager;

		explicit Window( WindowManagerHandle pWindowManager ) noexcept;
		virtual ~Window() noexcept;

		static WindowHandle create( WindowManagerHandle pWindowManager, const WindowCreateInfo & pCreateInfo );

		TS3_PCL_ATTR_NO_DISCARD WindowSize getClientAreaSize() const;
		TS3_PCL_ATTR_NO_DISCARD WindowSize getFrameSize() const;

	private:
		void _sysInitialize( const WindowCreateInfo & pCreateInfo );
		void _sysRelease() noexcept;
		void _sysGetClientAreaSize( WindowSize & pClientAreaSize ) const;
		void _sysGetFrameSize( WindowSize & pFrameSize ) const;
	};

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_WINDOW_H__
