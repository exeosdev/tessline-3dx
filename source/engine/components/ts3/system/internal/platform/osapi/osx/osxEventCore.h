
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

		enum NSAppEventID : NSUInteger
		{
			NSAppEventIDNull                      = 0,
			NSAppEventIDGenericLeftMouseDown      = NSEventTypeLeftMouseDown      ,
			NSAppEventIDGenericLeftMouseUp        = NSEventTypeLeftMouseUp        ,
			NSAppEventIDGenericRightMouseDown     = NSEventTypeRightMouseDown     ,
			NSAppEventIDGenericRightMouseUp       = NSEventTypeRightMouseUp       ,
			NSAppEventIDGenericMouseMoved         = NSEventTypeMouseMoved         ,
			NSAppEventIDGenericLeftMouseDragged   = NSEventTypeLeftMouseDragged   ,
			NSAppEventIDGenericRightMouseDragged  = NSEventTypeRightMouseDragged  ,
			NSAppEventIDGenericMouseEntered       = NSEventTypeMouseEntered       ,
			NSAppEventIDGenericMouseExited        = NSEventTypeMouseExited        ,
			NSAppEventIDGenericKeyDown            = NSEventTypeKeyDown            ,
			NSAppEventIDGenericKeyUp              = NSEventTypeKeyUp              ,
			NSAppEventIDGenericFlagsChanged       = NSEventTypeFlagsChanged       ,
			NSAppEventIDGenericAppKitDefined      = NSEventTypeAppKitDefined      ,
			NSAppEventIDGenericSystemDefined      = NSEventTypeSystemDefined      ,
			NSAppEventIDGenericApplicationDefined = NSEventTypeApplicationDefined ,
			NSAppEventIDGenericPeriodic           = NSEventTypePeriodic           ,
			NSAppEventIDGenericCursorUpdate       = NSEventTypeCursorUpdate       ,
			NSAppEventIDGenericRotate             = NSEventTypeRotate             ,
			NSAppEventIDGenericBeginGesture       = NSEventTypeBeginGesture       ,
			NSAppEventIDGenericEndGesture         = NSEventTypeEndGesture         ,
			NSAppEventIDX21                       ,
			NSAppEventIDGenericScrollWheel        = NSEventTypeScrollWheel        ,
			NSAppEventIDGenericTabletPoint        = NSEventTypeTabletPoint        ,
			NSAppEventIDGenericTabletProximity    = NSEventTypeTabletProximity    ,
			NSAppEventIDGenericOtherMouseDown     = NSEventTypeOtherMouseDown     ,
			NSAppEventIDGenericOtherMouseUp       = NSEventTypeOtherMouseUp       ,
			NSAppEventIDGenericOtherMouseDragged  = NSEventTypeOtherMouseDragged  ,
			NSAppEventIDX28                       ,
			NSAppEventIDGenericGesture            = NSEventTypeGesture            ,
			NSAppEventIDGenericMagnify            = NSEventTypeMagnify            ,
			NSAppEventIDGenericSwipe              = NSEventTypeSwipe              ,
			NSAppEventIDGenericSmartMagnify       = NSEventTypeSmartMagnify       ,
			NSAppEventIDGenericQuickLook          = NSEventTypeQuickLook          ,
			NSAppEventIDGenericPressure           = NSEventTypePressure           ,
			NSAppEventIDX35                       ,
			NSAppEventIDX36                       ,
			NSAppEventIDGenericDirectTouch        = NSEventTypeDirectTouch        ,
			NSAppEventIDGenericChangeMode         = NSEventTypeChangeMode         ,

			NSAppEventIDWindowDidExpose                  ,
			NSAppEventIDWindowDidMove                    ,
			NSAppEventIDWindowDidResize                  ,
			NSAppEventIDWindowMiniaturize                ,
			NSAppEventIDWindowDidDeminiaturize           ,
			NSAppEventIDWindowDidBecomeKey               ,
			NSAppEventIDWindowDidResignKey               ,
			NSAppEventIDWindowDidChangeBackingProperties ,
			NSAppEventIDWindowDidChangeScreenProfile     ,
			NSAppEventIDWindowWillEnterFullScreen        ,
			NSAppEventIDWindowDidEnterFullScreen         ,
			NSAppEventIDWindowWillExitFullScreen         ,
			NSAppEventIDWindowDidExitFullScreen          ,
			NSAppEventIDWindowWillClose                  ,
		};

		static_assert( NSAppEventIDX21 == 21 );
		static_assert( NSAppEventIDX28 == 28 );
		static_assert( NSAppEventIDX35 == 35 );
		static_assert( NSAppEventIDX36 == 36 );

		struct OSXEventSourceNativeData : public OSXNativeDataCommon
		{
			id nsWindow = nil;
			id nsView = nil;
			id nsEventListener = nil;
			OSXEventController * eventController = nullptr;
		};

		struct NativeEventType
		{
			NSAppEventID nsAppEventID = NSAppEventIDNull;

			NSEvent * nsEvent = nil;

			NSNotification * nsNotification = nil;

			NSWindow * nsSourceWindow = nil;

			NativeEventType( NSEvent * pNSEvent )
			: nsAppEventID( static_cast<NSAppEventID>( [pNSEvent type] ) )
			, nsEvent( pNSEvent )
			, nsSourceWindow( [pNSEvent window] )
			{}

			NativeEventType( NSAppEventID pAppEventID, NSNotification * pNSNotification )
			: nsAppEventID( pAppEventID )
			, nsNotification( pNSNotification )
			, nsSourceWindow( static_cast<NSWindow *>( [pNSNotification object] ) )
			{}
		};

		EventSource * osxFindEventSourceByNSWindow( OSXEventController & pEventController, NSWindow * pNSWindow );

		void osxCreateEventListener( OSXEventSourceNativeData & pEventSourceNativeData );

		bool osxIsEventTypeInputKeyboard( NSAppEventID pEventID );
		bool osxIsEventTypeInputMouse( NSAppEventID pEventID );
		bool osxIsEventTypeInputOther( NSAppEventID pEventID );
		bool osxIsEventTypeWindow( NSAppEventID pEventID );

		bool osxTranslateEvent( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent );

	}

	class OSXEventController : public OSXNativeObject<EventController, platform::OSXNativeDataCommon>
	{
	public:
		OSXEventController( SysContextHandle pSysContext );
		virtual ~OSXEventController() noexcept;

	private:
		/// @override EventController::_nativeRegisterEventSource
		virtual void _nativeRegisterEventSource( EventSource & pEventSource ) override final;

		/// @override EventController::_nativeUnregisterEventSource
		virtual void _nativeUnregisterEventSource( EventSource & pEventSource ) override final;

		/// @override EventController::_nativeDispatchPendingEvents
		virtual bool _nativeDispatchPendingEvents() override final;

		/// @override EventController::_nativeDispatchPendingEventsWait
		virtual bool _nativeDispatchPendingEventsWait() override final;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_EVENT_CORE_H__
