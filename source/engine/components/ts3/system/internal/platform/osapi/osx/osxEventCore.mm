
#include "osxEventCore.h"
#include "nsOSXApplicationProxy.h"
#include "nsOSXEventListener.h"
#include <ts3/system/internal/eventCorePrivate.h>

namespace ts3::system
{

	OSXEventController::OSXEventController( SysContextHandle pSysContext )
	: OSXNativeObject( std::move( pSysContext ) )
	{}

	OSXEventController::~OSXEventController() noexcept = default;

	void OSXEventController::_nativeRegisterEventSource( EventSource & pEventSource )
	{
	@autoreleasepool
	{
		auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::OSXEventSourceNativeData>();
		auto * nsEventListener = ( NSOSXEventListener * )eventSourceNativeData->nsEventListener;

		eventSourceNativeData->eventController = this;

		// nsEventListener->mEventSourceNativeData = eventSourceNativeData;

		[nsEventListener bind];
	}
	}

	void OSXEventController::_nativeUnregisterEventSource( EventSource & pEventSource )
	{
	}

	bool OSXEventController::_nativeDispatchPendingEvents()
	{
		for( NSEvent * nsEvent = nullptr; ; )
		{
			nsEvent = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
			if( nsEvent == nil )
			{
				return false;
			}

			// DISPATCH to controller

			[NSApp sendEvent:nsEvent];
		}

		return true;
	}

	bool OSXEventController::_nativeDispatchPendingEventsWait()
	{
		for( NSEvent * nsEvent = nullptr; ; )
		{
			nsEvent = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantFuture] inMode:NSDefaultRunLoopMode dequeue:YES];
			if( nsEvent == nil )
			{
				return false;
			}

			platform::NativeEventType osxNativeEvent;
			osxNativeEvent.nsEvent = nsEvent;

			platform::nativeEventDispatch( *this, osxNativeEvent );

			[NSApp sendEvent:nsEvent];
		}

		return true;
	}

	namespace platform
	{

		bool nativeEventTranslate( EventController & pEventController, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
		{
			return false;
		}

		void osxInitializeEventListener( OSXEventSourceNativeData & pEventSourceNativeData )
		{
		@autoreleasepool
		{
			@try
			{
				NSOSXEventListener * nsEventListener = [[NSOSXEventListener alloc] init];
				pEventSourceNativeData.nsEventListener = nsEventListener;
			}
			@catch( NSException * pException )
			{
				const auto message = [[pException reason] UTF8String];
				ts3DebugInterrupt();
			}
		}
		}

	}
	
}
