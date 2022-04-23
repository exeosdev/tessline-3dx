
#include "osxEventCore.h"
#include "nsOSXApplicationProxy.h"
#include "nsOSXEventListener.h"
#include "nsOSXWindow.h"
#include <ts3/system/internal/eventCorePrivate.h>

namespace ts3::system
{

	namespace platform
	{

		bool _osxTranslateEventInputKeyboard( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent );

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

		bool osxIsEventTypeInputKeyboard( NSAppEventID pEventID )
		{
			return ( pEventID >= NSAppEventIDGenericKeyDown ) && ( pEventID <= NSAppEventIDGenericFlagsChanged );
		}

		bool osxIsEventTypeInputMouse( NSAppEventID pEventID )
		{
			return ( ( pEventID >= NSAppEventIDGenericLeftMouseDown ) && ( pEventID <= NSAppEventIDGenericMouseExited ) ) ||
			       ( ( pEventID >= NSAppEventIDGenericScrollWheel ) && ( pEventID <= NSAppEventIDGenericOtherMouseDragged ) );
		}

		bool osxIsEventTypeInputOther( NSAppEventID pEventID )
		{
			return ( pEventID >= NSAppEventIDGenericGesture ) && ( pEventID <= NSAppEventIDGenericPressure );
		}

		bool osxIsEventTypeWindow( NSAppEventID pEventID )
		{
			return ( pEventID >= NSAppEventIDWindowDidExpose ) && ( pEventID <= NSAppEventIDWindowWillClose );
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

		bool _osxTranslateEventInputKeyboard( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent )
		{
			switch( pNativeEvent.nsAppEventID )
			{
				case NSAppEventIDGenericKeyDown:
				case NSAppEventIDGenericKeyUp:
				case NSAppEventIDGenericFlagsChanged:
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

		EMouseButtonID _osxQueryMouseButtonID( NSEvent * pNSEvent )
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

		math::Vec2i32 _osxQueryMouseRelativePosition( NSEvent * pNSEvent )
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

		EvtInputMouseMove & _osxReadMouseMoveEvent( const NativeEventType & pNativeEvent, EventObject & pOutEvent )
		{
			auto & eInputMouseMove = pOutEvent.eInputMouseMove;
			eInputMouseMove.eventCode = E_EVENT_CODE_INPUT_MOUSE_MOVE;
			eInputMouseMove.cursorPos = _osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
			return eInputMouseMove;
		}

		bool _osxTranslateEventInputMouse( const NativeEventType & pNativeEvent, EventSource & pEventSource, EventObject & pOutEvent )
		{
			switch( pNativeEvent.nsAppEventID )
			{
				case NSAppEventIDGenericLeftMouseDown:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = EMouseButtonID::Left;
					eInputMouseButton.cursorPos = _osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case NSAppEventIDGenericLeftMouseUp:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = EMouseButtonID::Left;
					eInputMouseButton.cursorPos = _osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case NSAppEventIDGenericRightMouseDown:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = EMouseButtonID::Right;
					eInputMouseButton.cursorPos = _osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case NSAppEventIDGenericRightMouseUp:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = EMouseButtonID::Right;
					eInputMouseButton.cursorPos = _osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case NSAppEventIDGenericMouseMoved:
				{
					auto & eInputMouseMove = _osxReadMouseMoveEvent( pNativeEvent, pOutEvent );
					break;
				}
				case NSAppEventIDGenericLeftMouseDragged:
				{
					auto & eInputMouseMove = _osxReadMouseMoveEvent( pNativeEvent, pOutEvent );
					eInputMouseMove.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_LEFT_BIT );
					break;
				}
				case NSAppEventIDGenericRightMouseDragged:
				{
					auto & eInputMouseMove = _osxReadMouseMoveEvent( pNativeEvent, pOutEvent );
					eInputMouseMove.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_RIGHT_BIT );
					break;
				}
				case NSAppEventIDGenericMouseEntered:
				case NSAppEventIDGenericMouseExited:
				case NSAppEventIDGenericScrollWheel:
				case NSAppEventIDGenericTabletPoint:
				case NSAppEventIDGenericTabletProximity:
				case NSAppEventIDGenericOtherMouseDown:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = _osxQueryMouseButtonID( pNativeEvent.nsEvent );
					eInputMouseButton.cursorPos = _osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case NSAppEventIDGenericOtherMouseUp:
				{
					auto & eInputMouseButton = pOutEvent.eInputMouseButton;
					eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = _osxQueryMouseButtonID( pNativeEvent.nsEvent );
					eInputMouseButton.cursorPos = _osxQueryMouseRelativePosition( pNativeEvent.nsEvent );
					break;
				}
				case NSAppEventIDGenericOtherMouseDragged:
				{
					auto & eInputMouseMove = _osxReadMouseMoveEvent( pNativeEvent, pOutEvent );
					const auto buttonID = _osxQueryMouseButtonID( pNativeEvent.nsEvent );
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

			if( pOutEvent.code != E_EVENT_CODE_UNDEFINED )
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
				case NSAppEventIDGenericGesture:
				case NSAppEventIDGenericMagnify:
				case NSAppEventIDGenericSwipe:
				case NSAppEventIDGenericSmartMagnify:
				case NSAppEventIDGenericQuickLook:
				case NSAppEventIDGenericPressure:
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
				case NSAppEventIDWindowDidExpose:
				case NSAppEventIDWindowDidMove:
				case NSAppEventIDWindowDidResize:
				case NSAppEventIDWindowMiniaturize:
				case NSAppEventIDWindowDidDeminiaturize:
				case NSAppEventIDWindowDidBecomeKey:
				case NSAppEventIDWindowDidResignKey:
				case NSAppEventIDWindowDidChangeBackingProperties:
				case NSAppEventIDWindowDidChangeScreenProfile:
				case NSAppEventIDWindowWillEnterFullScreen:
				case NSAppEventIDWindowDidEnterFullScreen:
				case NSAppEventIDWindowWillExitFullScreen:
				case NSAppEventIDWindowDidExitFullScreen:
				{
					break;
				}
				case NSAppEventIDWindowWillClose:
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
