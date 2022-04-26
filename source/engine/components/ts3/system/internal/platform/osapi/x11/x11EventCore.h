
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__

#include "x11Common.h"
#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>

namespace ts3::system
{

	class X11EventController;

	namespace platform
	{

		enum class EX11MouseButtonID : enum_default_value_t
		{
			Unknown,
			Left,
			Middle,
			Right,
			VWheelUp,
			VWheelDown,
			HWheelLeft,
			HWheelRight,
			XBT1,
			XBT2
		};

		enum EX11SystemWindowFlags : uint32
		{
			E_X11_SYSTEM_WINDOW_FLAG_WM_STATE_FULLSCREEN = 0x8000
		};

		struct X11EventSourceNativeData : public X11NativeDataCommon
		{
			XWindow windowXID = E_X11_XID_NONE;

			Bitmask<uint32> sysWindowFlags = 0;
		};

		struct NativeEventType
		{
			XEvent xEvent;
		};

		TS3_SYSTEM_API_NODISCARD EventSource * x11FindEventSourceByXWindow( X11EventController & pEventController, XWindow pWindowXID );

		bool x11TranslateEvent( const XEvent & pXEvent, EventSystemSharedState & pSharedState, EventSource & pEventSource, EventObject & pOutEvent );

	}

	class X11EventController : public X11NativeObject<EventController, platform::X11NativeDataCommon>
	{
	public:
		X11EventController( SysContextHandle pSysContext );
		virtual ~X11EventController() noexcept;

		using EventController::getEventDispatcherInputState;
		using EventController::getEventDispatcherConfig;

	private:
		/// @copybrief EventController::_nativeRegisterEventSource
		virtual void _nativeRegisterEventSource( EventSource & pEventSource ) override final;

		/// @copybrief EventController::_nativeUnregisterEventSource
		virtual void _nativeUnregisterEventSource( EventSource & pEventSource ) override final;

		/// @copybrief EventController::_nativeDispatchPendingEvents
		virtual bool _nativeDispatchPendingEvents() override final;

		/// @copybrief EventController::_nativeDispatchPendingEventsWait
		virtual bool _nativeDispatchPendingEventsWait() override final;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
