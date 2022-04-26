
#include "osxEventCore.h"
#include "nsOSXEventListener.h"
#include "nsOSXWindow.h"
#include <ts3/system/internal/eventCorePrivate.h>

namespace ts3::system
{

	namespace platform
	{

		bool _osxTranslateEventInputKeyboard( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent );

		EvtInputMouseMove & _osxReadMouseMoveEvent( const NativeEventType & pNativeEvent, EventObject & pOutEvent );

		bool _osxTranslateEventInputMouse( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent );

		bool _osxTranslateEventInputOther( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent );

		bool _osxTranslateEventWindow( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent );

	}

	OSXEventController::OSXEventController( SysContextHandle pSysContext )
	: OSXNativeObject( std::move( pSysContext ) )
	{}

	OSXEventController::~OSXEventController() noexcept = default;

	void OSXEventController::_nativeRegisterEventSource( EventSource & pEventSource )
	{
	@autoreleasepool
	{
		auto * eventSourceNativeData =
			pEventSource.getEventSourceNativeDataAs<platform::OSXEventSourceNativeData>();

		auto * nsEventListener = ( NSOSXEventListener * )eventSourceNativeData->nsEventListener;

		[nsEventListener bind:this];
	}
	}

	void OSXEventController::_nativeUnregisterEventSource( EventSource & pEventSource )
	{
	@autoreleasepool
	{
		auto * eventSourceNativeData =
				pEventSource.getEventSourceNativeDataAs<platform::OSXEventSourceNativeData>();

		auto * nsEventListener = ( NSOSXEventListener * )eventSourceNativeData->nsEventListener;

		[nsEventListener unbind];
	}
	}

	bool OSXEventController::_nativeDispatchPendingEvents()
	{
		for( NSEvent * nsEvent = nullptr; ; )
		{
			nsEvent = [NSApp nextEventMatchingMask:NSEventMaskAny
			                 untilDate:[NSDate distantPast]
			                 inMode:NSDefaultRunLoopMode
			                 dequeue:YES];

			if( nsEvent == nil )
			{
				return false;
			}

			[NSApp sendEvent:nsEvent];
		}

		return true;
	}

	bool OSXEventController::_nativeDispatchPendingEventsWait()
	{
		for( NSEvent * nsEvent = nullptr; ; )
		{
			nsEvent = [NSApp nextEventMatchingMask:NSEventMaskAny
			                 untilDate:[NSDate distantFuture]
			                 inMode:NSDefaultRunLoopMode
			                 dequeue:YES];

			if( nsEvent == nil )
			{
				return false;
			}

			[NSApp sendEvent:nsEvent];
		}

		return true;
	}

	namespace platform
	{

		bool nativeEventTranslate( EventController & pEventController, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
		{
			auto * osxEventController = pEventController.queryInterface<OSXEventController>();

			if( auto * eventSource = osxFindEventSourceByNSWindow( *osxEventController, pNativeEvent.nsSourceWindow ) )
			{
				return osxTranslateEvent( pNativeEvent, *eventSource, pOutEvent );
			}
			
			return false;
		}

		EventSource * osxFindEventSourceByNSWindow( OSXEventController & pEventController, NSWindow * pNSWindow )
		{
			auto * eventSource = pEventController.findEventSource( [pNSWindow]( const EventSource & pEventSource ) -> bool {
				const auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::OSXEventSourceNativeData>();
				return eventSourceNativeData->nsWindow == pNSWindow;
			});
			return eventSource;
		}

		void osxCreateEventListener( OSXEventSourceNativeData & pEventSourceNativeData )
		{
		@autoreleasepool
		{
			if( ![( id )pEventSourceNativeData.nsWindow isKindOfClass:[NSOSXWindow class]] )
			{
				return;
			}

			auto * nsWindow = ( NSOSXWindow * )( pEventSourceNativeData.nsWindow );

			@try
			{
				auto * nsEventListener = [[NSOSXEventListener alloc] initForNSWindow:nsWindow];
				pEventSourceNativeData.nsEventListener = nsEventListener;
			}
			@catch( NSException * pException )
			{
				const auto message = [[pException reason] UTF8String];
				ts3DebugInterrupt();
			}
		}
		}

		bool osxTranslateEvent( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent )
		{
			if( osxIsEventTypeInputKeyboard( pNativeEvent.nsAppEventID ) )
			{
				if( _osxTranslateEventInputKeyboard( pNativeEvent, pEventSource, pOutEvent ) )
				{
					return true;
				}
			}
			else if( osxIsEventTypeInputMouse( pNativeEvent.nsAppEventID ) )
			{
				if( _osxTranslateEventInputMouse( pNativeEvent, pEventSource, pOutEvent ) )
				{
					return true;
				}
			}
			else if( osxIsEventTypeInputOther( pNativeEvent.nsAppEventID ) )
			{
				if( _osxTranslateEventInputOther( pNativeEvent, pEventSource, pOutEvent ) )
				{
					return true;
				}
			}
			else if( osxIsEventTypeWindow( pNativeEvent.nsAppEventID ) )
			{
				if( _osxTranslateEventWindow( pNativeEvent, pEventSource, pOutEvent ) )
				{
					return true;
				}
			}

			return false;
		}

		bool osxIsEventTypeInputKeyboard( OSXEventID pEventID )
		{
			return ( pEventID >= OSXEventIDGenericKeyDown ) && ( pEventID <= OSXEventIDGenericFlagsChanged );
		}

		bool osxIsEventTypeInputMouse( OSXEventID pEventID )
		{
			return ( ( pEventID >= OSXEventIDGenericLeftMouseDown ) && ( pEventID <= OSXEventIDGenericMouseExited ) ) ||
			       ( ( pEventID >= OSXEventIDGenericScrollWheel ) && ( pEventID <= OSXEventIDGenericOtherMouseDragged ) );
		}

		bool osxIsEventTypeInputOther( OSXEventID pEventID )
		{
			return ( pEventID >= OSXEventIDGenericGesture ) && ( pEventID <= OSXEventIDGenericPressure );
		}

		bool osxIsEventTypeWindow( OSXEventID pEventID )
		{
			return ( pEventID >= OSXEventIDWindowDidExpose ) && ( pEventID <= OSXEventIDWindowWillClose );
		}

		EMouseButtonID osxQueryMouseButtonID( NSEvent * pNSEvent )
		{
			static constexpr EMouseButtonID sMouseButtonIDArray[] =
					{
							/* 0 */ EMouseButtonID::Left,
							/* 1 */ EMouseButtonID::Right,
							/* 2 */ EMouseButtonID::Middle,
							/* 3 */ EMouseButtonID::XB1,
							/* 4 */ EMouseButtonID::XB2,
					};
			return staticArrayElement( sMouseButtonIDArray, [pNSEvent buttonNumber] );
		}

		math::Vec2i32 osxQueryMouseRelativePosition( NSEvent * pNSEvent )
		{
			auto * nsSourceWindow = [pNSEvent window];

			const auto & sourceViewRect = [[nsSourceWindow contentView] frame];
			const auto & mouseLocation = [pNSEvent locationInWindow];

			math::Vec2i32 result {
				static_cast<int32>( std::round( mouseLocation.x ) ),
				static_cast<int32>( std::round( sourceViewRect.size.height - mouseLocation.y ) )
			};

			return result;
		}

		Bitmask<EOSXMouseScrollEventFlags> osxQueryMouseScrollEventFlags( const NativeEventType & pNativeEvent )
		{
			Bitmask<EOSXMouseScrollEventFlags> scrollEventMask = 0;

			if( [pNativeEvent.nsEvent respondsToSelector:@selector(hasPreciseScrollingDeltas)] )
			{
				if( [pNativeEvent.nsEvent hasPreciseScrollingDeltas] )
				{
					scrollEventMask.set( E_OSX_MOUSE_SCROLL_EVENT_FLAG_SCROLLING_MODE_PRECISE_BIT );
				}
				else
				{
					scrollEventMask.set( E_OSX_MOUSE_SCROLL_EVENT_FLAG_SCROLLING_MODE_NORMAL_BIT );
				}
			}

			if( [pNativeEvent.nsEvent respondsToSelector:@selector(isDirectionInvertedFromDevice)] )
			{
				if( [pNativeEvent.nsEvent isDirectionInvertedFromDevice] )
				{
					scrollEventMask.set( E_OSX_MOUSE_SCROLL_EVENT_FLAG_SCROLLING_DIRECTION_INVERTED_BIT );
				}
				else
				{
					scrollEventMask.set( E_OSX_MOUSE_SCROLL_EVENT_FLAG_SCROLLING_DIRECTION_NORMAL_BIT );
				}
			}

			return scrollEventMask;
		}

		bool _osxTranslateEventInputKeyboard( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent )
		{
			switch( pNativeEvent.nsAppEventID )
			{
				case OSXEventIDGenericKeyDown:
				case OSXEventIDGenericKeyUp:
				case OSXEventIDGenericFlagsChanged:
				{
					break;
				}
				default:
				{
					ts3DebugInterrupt();
					return false;
				}
			}

			return true;
		}

		EvtInputMouseMove & _osxReadMouseMoveEvent( const NativeEventType & pNativeEvent, EventObject & pOutEvent )
		{
			auto & eInputMouseMove = pOutEvent.eInputMouseMove;
			eInputMouseMove.eventCode = E_EVENT_CODE_INPUT_MOUSE_MOVE;
			eInputMouseMove.cursorPos = osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
			return eInputMouseMove;
		}

		bool _osxTranslateEventInputMouse( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent )
		{
			switch( pNativeEvent.nsAppEventID )
			{
				case OSXEventIDGenericLeftMouseDown:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = EMouseButtonID::Left;
					eInputMouseButton.cursorPos = osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case OSXEventIDGenericLeftMouseUp:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = EMouseButtonID::Left;
					eInputMouseButton.cursorPos = osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case OSXEventIDGenericRightMouseDown:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = EMouseButtonID::Right;
					eInputMouseButton.cursorPos = osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case OSXEventIDGenericRightMouseUp:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = EMouseButtonID::Right;
					eInputMouseButton.cursorPos = osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case OSXEventIDGenericMouseMoved:
				{
					auto & eInputMouseMove = _osxReadMouseMoveEvent( pNativeEvent, pOutEvent );
					break;
				}
				case OSXEventIDGenericLeftMouseDragged:
				{
					auto & eInputMouseMove = _osxReadMouseMoveEvent( pNativeEvent, pOutEvent );
					eInputMouseMove.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_LEFT_BIT );
					break;
				}
				case OSXEventIDGenericRightMouseDragged:
				{
					auto & eInputMouseMove = _osxReadMouseMoveEvent( pNativeEvent, pOutEvent );
					eInputMouseMove.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_RIGHT_BIT );
					break;
				}
				case OSXEventIDGenericMouseEntered:
				case OSXEventIDGenericMouseExited:
				{
					break;
				}
				case OSXEventIDGenericScrollWheel:
				{
					const auto scrollDeltaX = [pNativeEvent.nsEvent scrollingDeltaX];
					const auto scrollDeltaY = [pNativeEvent.nsEvent scrollingDeltaY];

					auto & eInputMouseScroll = pOutEvent.eInputMouseScroll;
					eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
					eInputMouseScroll.cursorPos = osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					eInputMouseScroll.scrollDelta.x = -scrollDeltaX;
					eInputMouseScroll.scrollDelta.y = scrollDeltaY;
					eInputMouseScroll.scrollDirection = EMouseScrollDirection::Normal;

					const auto scrollEventMask = osxQueryMouseScrollEventFlags( pNativeEvent );

					if( scrollEventMask.isSet( E_OSX_MOUSE_SCROLL_EVENT_FLAG_SCROLLING_MODE_NORMAL_BIT ) )
					{
						eInputMouseScroll.scrollDelta.x *= 0.1;
						eInputMouseScroll.scrollDelta.y *= 0.1;
					}

					if( scrollEventMask.isSet( E_OSX_MOUSE_SCROLL_EVENT_FLAG_SCROLLING_DIRECTION_INVERTED_BIT ) )
					{
						eInputMouseScroll.scrollDirection = EMouseScrollDirection::Inverted;
					}

					break;
				}
				case OSXEventIDGenericTabletPoint:
				case OSXEventIDGenericTabletProximity:
				{
					break;
				}
				case OSXEventIDGenericOtherMouseDown:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = osxQueryMouseButtonID( pNativeEvent.nsEvent );
					eInputMouseButton.cursorPos = osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case OSXEventIDGenericOtherMouseUp:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = osxQueryMouseButtonID( pNativeEvent.nsEvent );
					eInputMouseButton.cursorPos = osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case OSXEventIDGenericOtherMouseDragged:
				{
					auto & eInputMouseMove = _osxReadMouseMoveEvent( pNativeEvent, pOutEvent );
					const auto buttonID = osxQueryMouseButtonID( pNativeEvent.nsEvent );
					const auto buttonFlag = ecGetMouseButtonFlagFromButtonID( buttonID );
					eInputMouseMove.buttonStateMask.set( buttonFlag );
					break;
				}
				default:
				{
					ts3DebugInterrupt();
					return false;
				}
			}

			auto cxEnableMouseEventReporting = false;
			if( ( pOutEvent.code != E_EVENT_CODE_UNDEFINED ) && cxEnableMouseEventReporting )
			{
				printf( "Mouse event (%u) at: [%d , %d]\n",
				        (uint32)pNativeEvent.nsAppEventID,
				        pOutEvent.eInputMouse.cursorPos.x,
				        pOutEvent.eInputMouse.cursorPos.y );
			}

			return true;
		}

		bool _osxTranslateEventInputOther( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent )
		{
			switch( pNativeEvent.nsAppEventID )
			{
				case OSXEventIDGenericGesture:
				case OSXEventIDGenericMagnify:
				case OSXEventIDGenericSwipe:
				case OSXEventIDGenericSmartMagnify:
				case OSXEventIDGenericQuickLook:
				case OSXEventIDGenericPressure:
				{
					break;
				}
				default:
				{
					ts3DebugInterrupt();
					return false;
				}
			}

			return true;
		}

		bool _osxTranslateEventWindow( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent )
		{
			switch( pNativeEvent.nsAppEventID )
			{
				case OSXEventIDWindowDidExpose:
				case OSXEventIDWindowDidMove:
				case OSXEventIDWindowDidResize:
				case OSXEventIDWindowMiniaturize:
				case OSXEventIDWindowDidDeminiaturize:
				case OSXEventIDWindowDidBecomeKey:
				case OSXEventIDWindowDidResignKey:
				case OSXEventIDWindowDidChangeBackingProperties:
				case OSXEventIDWindowDidChangeScreenProfile:
				case OSXEventIDWindowWillEnterFullScreen:
				case OSXEventIDWindowDidEnterFullScreen:
				case OSXEventIDWindowWillExitFullScreen:
				case OSXEventIDWindowDidExitFullScreen:
				{
					break;
				}
				case OSXEventIDWindowWillClose:
				{
					auto & eWindowUpdateDestroy = pOutEvent.eWindowUpdateDestroy;
					eWindowUpdateDestroy.eventCode = E_EVENT_CODE_WINDOW_UPDATE_DESTROY;
					eWindowUpdateDestroy.eventSource = &pEventSource;
					break;
				}
				default:
				{
					ts3DebugInterrupt();
					return false;
				}
			}

			return true;
		}

	}
	
}
