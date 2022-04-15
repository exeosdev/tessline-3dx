
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_EVENT_CORE_H__

#include "osxCommon.h"
#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>

#import <AppKit/NSEvent.h>

namespace ts3::system
{

	class OSXEventController;

	namespace platform
	{

		struct OSXEventSourceNativeData : public OSXNativeDataCommon
		{
			id nsWindow = nil;

			id nsView = nil;

			id nsEventListener = nil;

			OSXEventController * eventController = nullptr;
		};

		struct NativeEventType
		{
			NSEvent * nsEvent = nil;
		};

		void osxInitializeEventListener( OSXEventSourceNativeData & pEventSourceNativeData );

		void osxReleaseEventListener( OSXEventSourceNativeData & pEventSourceNativeData );

	}

	class OSXEventController : public OSXNativeObject<EventController, platform::OSXNativeDataCommon>
	{
	public:
		OSXEventController( SysContextHandle pSysContext );
		virtual ~OSXEventController() noexcept;

		using EventController::getEventSystemInternalConfig;
		using EventController::getEventDispatcherInputState;

	private:
		/// @override EventController::_nativeRegisterEventSource
		virtual void _nativeRegisterEventSource( EventSource & pEventSource ) override final;

		/// @override EventController::_nativeUnregisterEventSource
		virtual void _nativeUnregisterEventSource( EventSource & pEventSource ) override final;

		/// @override EventController::_nativeUpdateSysQueue
		virtual bool _nativeUpdateSysQueue() override final;

		/// @override EventController::_nativeUpdateSysQueueWait
		virtual bool _nativeUpdateSysQueueWait() override final;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_EVENT_CORE_H__
