
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__

#include "win32Common.h"
#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>
#include <windowsx.h> // For GET_X_LPARAM/GET_Y_LPARAM

namespace ts3::system
{

	class Win32EventController;

	namespace platform
	{

		struct Win32EventSourceNativeData
		{
			HWND hwnd = nullptr;

			Bitmask<uint32> sysWindowFlags = 0;
		};

		struct Win32EventSourceState
		{
			LONG_PTR savedEventCallback = 0;
			LONG_PTR savedEventCallbackUserData = 0;
			EventController * eventController = nullptr;
		};

		struct NativeEventType : public MSG
		{};

		TS3_SYSTEM_API_NODISCARD EventSource * win32FindEventSourceByHWND( Win32EventController & pEventController, HWND pHWND );

		bool win32TranslateEvent( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );

		// Default event procedure for all windows created by the system.
		LRESULT __stdcall win32DefaultWindowEventCallback( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );

	}

	class Win32EventController : public Win32NativeObject<EventController, void>
	{
	public:
		Win32EventController( SysContextHandle pSysContext );
		virtual ~Win32EventController() noexcept;

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

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__
