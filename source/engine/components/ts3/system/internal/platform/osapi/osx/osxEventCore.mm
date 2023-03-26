
#include "osxEventCore.h"
#include "nsOSXEventListener.h"
#include "nsOSXWindow.h"
#include "nsOSXKeyboardSupport.h"
#include <ts3/system/internal/eventCorePrivate.h>

namespace ts3::system
{

	namespace platform
	{

		void _osxTranslateEventInputKeyboard( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent );

		bool _osxReadMouseMoveEvent( EventController & pEventController, const NativeEventType & pNativeEvent, EventObject & pOutEvent );

		void _osxTranslateEventInputMouse( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent );

		void _osxTranslateEventInputOther( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent );

		void _osxTranslateEventWindow( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent );

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
				return osxTranslateEvent( pEventController, *eventSource, pNativeEvent, pOutEvent );
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

		bool osxTranslateEvent( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
		{
			if( osxIsEventTypeInputKeyboard( pNativeEvent.nsAppEventID ) )
			{
				_osxTranslateEventInputKeyboard( pEventController, pEventSource, pNativeEvent, pOutEvent );
			}
			else if( osxIsEventTypeInputMouse( pNativeEvent.nsAppEventID ) )
			{
				_osxTranslateEventInputMouse( pEventController, pEventSource, pNativeEvent, pOutEvent );
			}
			else if( osxIsEventTypeInputOther( pNativeEvent.nsAppEventID ) )
			{
				_osxTranslateEventInputOther( pEventController, pEventSource, pNativeEvent, pOutEvent );
			}
			else if( osxIsEventTypeWindow( pNativeEvent.nsAppEventID ) )
			{
				_osxTranslateEventWindow( pEventController, pEventSource, pNativeEvent, pOutEvent );
			}

			return pOutEvent.code != E_EVENT_CODE_UNDEFINED;
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

		struct OSXMouseCursorState
		{
			CGPoint positionScreenSpace;
			CGPoint positionWindowSpace;
			bool isInsideWindowFrame = false;
			bool isInsideWindowClientArea = false;

			bool isInsideWindow() const noexcept
			{
				return isInsideWindowClientArea;
			}

			MouseCursorPos positionScreenSpaceI32() const
			{
				return {
					static_cast<int32>( std::round( positionScreenSpace.x ) ),
					static_cast<int32>( std::round( positionScreenSpace.y ) )
				};
			}

			MouseCursorPos positionWindowSpaceI32() const
			{
				return {
					static_cast<int32>( std::round( positionWindowSpace.x ) ),
					static_cast<int32>( std::round( positionWindowSpace.y ) )
				};
			}
		};

		OSXMouseCursorState osxQueryMouseCursorState( NSEvent * pNSEvent )
		{
			auto * nsSourceWindow = [pNSEvent window];
			const auto nsWindowFrame = [nsSourceWindow frame];
			const auto nsWindowStyle = [nsSourceWindow styleMask];

			const auto nsWindowClientAreaSize =
					[NSWindow contentRectForFrameRect:nsWindowFrame
					                        styleMask:nsWindowStyle];

			const auto mousePositionScreenSpace = [NSEvent mouseLocation];
			const auto mousePositionWindowSpace = [pNSEvent locationInWindow];
			bool mouseIsInsideWindowFrame = false;
			bool mouseIsInsideWindowClientArea = false;

			if( checkValueInsideInclusiveRange( { 0.0, nsWindowClientAreaSize.size.width }, mousePositionWindowSpace.x ) &&
			    checkValueInsideInclusiveRange( { 0.0, nsWindowClientAreaSize.size.height }, mousePositionWindowSpace.y ) )
			{
				mouseIsInsideWindowFrame = true;
				mouseIsInsideWindowClientArea = true;
			}
			else if( checkValueInsideInclusiveRange( { 0.0, nsWindowFrame.size.width }, mousePositionWindowSpace.x ) &&
			         checkValueInsideInclusiveRange( { 0.0, nsWindowFrame.size.height }, mousePositionWindowSpace.y ) )
			{
				mouseIsInsideWindowFrame = true;
			}

			OSXMouseCursorState osxCursorState;
			osxCursorState.positionScreenSpace = mousePositionScreenSpace;
			osxCursorState.positionWindowSpace = mousePositionWindowSpace;
			osxCursorState.isInsideWindowFrame = mouseIsInsideWindowFrame;
			osxCursorState.isInsideWindowClientArea = mouseIsInsideWindowClientArea;

			return osxCursorState;
		}

		Bitmask<EMouseButtonFlagBits> queryMouseButtonsActiveMask( NSEvent * pNSEvent )
		{
			const auto nsButtonMask = [NSEvent pressedMouseButtons];
			ts3DebugOutputFmt( "MouseButtonMask: %d", nsButtonMask );
			return nsButtonMask;
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

		EKeyCode _osxGetSysKeyCodeV1( unsigned short pNSKeyCode );
		EKeyCode _osxGetSysKeyCodeV2( unsigned short pNSKeyCode );

		void _osxTranslateEventInputKeyboard( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
		{
			auto & inputKeyboardState = pEventController.getEventSystemSharedState().inputKeyboardState;

			switch( pNativeEvent.nsAppEventID )
			{
				case OSXEventIDGenericKeyDown:
				{
					auto & eInputKeyboard = pOutEvent.eInputKeyboard;
					eInputKeyboard.eventCode = E_EVENT_CODE_INPUT_KEYBOARD;
					inputKeyboardState.activeModifiersMask = osxTranslateModifierKeyFlags( [pNativeEvent.nsEvent modifierFlags] );
					eInputKeyboard.inputKeyboardState = &inputKeyboardState;
					eInputKeyboard.keyAction = EKeyActionType::Press;
					eInputKeyboard.keyCode = _osxGetSysKeyCodeV1( [pNativeEvent.nsEvent keyCode] );
					break;
				}
				case OSXEventIDGenericKeyUp:
				{
					auto & eInputKeyboard = pOutEvent.eInputKeyboard;
					eInputKeyboard.eventCode = E_EVENT_CODE_INPUT_KEYBOARD;
					inputKeyboardState.activeModifiersMask = osxTranslateModifierKeyFlags( [pNativeEvent.nsEvent modifierFlags] );
					eInputKeyboard.inputKeyboardState = &inputKeyboardState;
					eInputKeyboard.keyAction = EKeyActionType::Release;
					eInputKeyboard.keyCode = _osxGetSysKeyCodeV1( [pNativeEvent.nsEvent keyCode] );
					break;
				}
				case OSXEventIDGenericFlagsChanged:
				{
					const auto currentModifierFlags = osxTranslateModifierKeyFlags( [pNativeEvent.nsEvent modifierFlags] );
					const auto previousModifierFlags = inputKeyboardState.activeModifiersMask;
					const auto modifierDiff = currentModifierFlags ^ previousModifierFlags;
					const auto modifierKeyCode = evt::getModifierKeyCodeFromModifierFlags( modifierDiff );

					auto & eInputKeyboard = pOutEvent.eInputKeyboard;
					eInputKeyboard.eventCode = E_EVENT_CODE_INPUT_KEYBOARD;
					inputKeyboardState.activeModifiersMask = currentModifierFlags;
					eInputKeyboard.inputKeyboardState = &inputKeyboardState;
					eInputKeyboard.keyCode = evt::getModifierKeyCodeFromModifierFlags( modifierDiff );

					if( !previousModifierFlags.isSet( modifierDiff ) )
					{
						eInputKeyboard.keyAction = EKeyActionType::Press;
					}
					else
					{
						eInputKeyboard.keyAction = EKeyActionType::Release;
					}

					break;
				}
				default:
				{
					ts3DebugInterrupt();
				}
			}
		}

		EvtInputMouseScroll & osxProcessMouseScrollEvent( EventController & pEventController, const OSXMouseCursorState & pCursorState, EventObject & pOutEvent )
		{
			auto & controllerNativeData = pEventController.getInterface<OSXEventController>()->mNativeData;
			auto & inputMouseState = pEventController.getEventSystemSharedState().inputMouseState;
			auto & eInputMouseScroll = pOutEvent.eInputMouseScroll;

			if( inputMouseState.lastCursorPos == CX_EVENT_MOUSE_POS_INVALID )
			{
				controllerNativeData.lastCursorPosReal = pCursorState.positionScreenSpace;
				inputMouseState.lastCursorPos = pCursorState.positionScreenSpaceI32();
			}

			eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
			eInputMouseScroll.cursorPos = pCursorState.positionScreenSpaceI32();
			eInputMouseScroll.inputMouseState = &inputMouseState;

			controllerNativeData.lastCursorPosReal = pCursorState.positionScreenSpace;
			inputMouseState.lastCursorPos = pCursorState.positionScreenSpaceI32();

			return eInputMouseScroll;
		}

		EvtInputMouseButton & osxProcessMouseButtonEvent( EventController & pEventController, const OSXMouseCursorState & pCursorState, EventObject & pOutEvent )
		{
			auto & controllerNativeData = pEventController.getInterface<OSXEventController>()->mNativeData;
			auto & inputMouseState = pEventController.getEventSystemSharedState().inputMouseState;
			auto & eInputMouseButton = pOutEvent.eInputMouseButton;

			if( inputMouseState.lastCursorPos == CX_EVENT_MOUSE_POS_INVALID )
			{
				controllerNativeData.lastCursorPosReal = pCursorState.positionScreenSpace;
				inputMouseState.lastCursorPos = pCursorState.positionScreenSpaceI32();
			}

			eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
			eInputMouseButton.cursorPos = pCursorState.positionScreenSpaceI32();
			eInputMouseButton.inputMouseState = &inputMouseState;

			controllerNativeData.lastCursorPosReal = pCursorState.positionScreenSpace;
			inputMouseState.lastCursorPos = pCursorState.positionScreenSpaceI32();

			return eInputMouseButton;
		}

		EvtInputMouseMove & osxProcessMouseMoveEvent( EventController & pEventController, const OSXMouseCursorState & pCursorState, EventObject & pOutEvent )
		{
			auto & controllerNativeData = pEventController.getInterface<OSXEventController>()->mNativeData;
			auto & inputMouseState = pEventController.getEventSystemSharedState().inputMouseState;
			auto & eInputMouseMove = pOutEvent.eInputMouseMove;

			if( inputMouseState.lastCursorPos == CX_EVENT_MOUSE_POS_INVALID )
			{
				controllerNativeData.lastCursorPosReal = pCursorState.positionScreenSpace;
				inputMouseState.lastCursorPos = pCursorState.positionScreenSpaceI32();
			}

			eInputMouseMove.eventCode = E_EVENT_CODE_INPUT_MOUSE_MOVE;
			eInputMouseMove.cursorPos = pCursorState.positionScreenSpaceI32();
			eInputMouseMove.movementDelta.x = static_cast<int32>( std::round( pCursorState.positionScreenSpace.x - inputMouseState.lastCursorPos.x ) );
			eInputMouseMove.movementDelta.y = static_cast<int32>( std::round( pCursorState.positionScreenSpace.y - inputMouseState.lastCursorPos.y ) );
			eInputMouseMove.inputMouseState = &inputMouseState;

			controllerNativeData.lastCursorPosReal = pCursorState.positionScreenSpace;
			inputMouseState.lastCursorPos = pCursorState.positionScreenSpaceI32();

			return eInputMouseMove;
		}

		void _osxTranslateEventInputMouse( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
		{
			const auto mouseCursorState = osxQueryMouseCursorState( pNativeEvent.nsEvent );

			if( !mouseCursorState.isInsideWindow() )
			{
				auto & inputMouseState = pEventController.getEventSystemSharedState().inputMouseState;
				inputMouseState.lastCursorPos = CX_EVENT_MOUSE_POS_INVALID;
				return;
			}

			switch( pNativeEvent.nsAppEventID )
			{
				case OSXEventIDGenericLeftMouseDown:
				{
					auto & eInputMouseButton = osxProcessMouseButtonEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = EMouseButtonID::Left;
					break;
				}
				case OSXEventIDGenericLeftMouseUp:
				{
					auto & eInputMouseButton = osxProcessMouseButtonEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = EMouseButtonID::Left;
					break;
				}
				case OSXEventIDGenericRightMouseDown:
				{
					auto & eInputMouseButton = osxProcessMouseButtonEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = EMouseButtonID::Right;
					break;
				}
				case OSXEventIDGenericRightMouseUp:
				{
					auto & eInputMouseButton = osxProcessMouseButtonEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = EMouseButtonID::Right;
					break;
				}
				case OSXEventIDGenericMouseMoved:
				{
					auto & eInputMouseMove = osxProcessMouseMoveEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseMove.buttonStateMask = 0;
					break;
				}
				case OSXEventIDGenericLeftMouseDragged:
				{
					auto & eInputMouseMove = osxProcessMouseMoveEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseMove.buttonStateMask = E_MOUSE_BUTTON_FLAG_LEFT_BIT;
					break;
				}
				case OSXEventIDGenericRightMouseDragged:
				{
					auto & eInputMouseMove = osxProcessMouseMoveEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseMove.buttonStateMask = E_MOUSE_BUTTON_FLAG_RIGHT_BIT;
					break;
				}
				case OSXEventIDGenericMouseEntered:
				case OSXEventIDGenericMouseExited:
				{
					break;
				}
				case OSXEventIDGenericScrollWheel:
				{
					auto & eInputMouseScroll = osxProcessMouseScrollEvent( pEventController, mouseCursorState, pOutEvent );
					{
						const auto scrollDeltaX = [pNativeEvent.nsEvent scrollingDeltaX];
						const auto scrollDeltaY = [pNativeEvent.nsEvent scrollingDeltaY];

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
					auto & eInputMouseButton = osxProcessMouseButtonEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
					eInputMouseButton.buttonID = osxQueryMouseButtonID( pNativeEvent.nsEvent );
					break;
				}
				case OSXEventIDGenericOtherMouseUp:
				{
					auto & eInputMouseButton = osxProcessMouseButtonEvent( pEventController, mouseCursorState, pOutEvent );
					eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
					eInputMouseButton.buttonID = osxQueryMouseButtonID( pNativeEvent.nsEvent );
					break;
				}
				case OSXEventIDGenericOtherMouseDragged:
				{
					auto & eInputMouseMove = osxProcessMouseMoveEvent( pEventController, mouseCursorState, pOutEvent );
					const auto buttonID = osxQueryMouseButtonID( pNativeEvent.nsEvent );
					const auto buttonFlag = cxdefs::getMouseButtonFlagFromButtonID( buttonID );
					eInputMouseMove.buttonStateMask = buttonFlag;
					break;
				}
				default:
				{
					ts3DebugInterrupt();
				}
			}
		}

		void _osxTranslateEventInputOther( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
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
				}
			}
		}

		void _osxTranslateEventWindow( EventController & pEventController, EventSource & pEventSource, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
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
				}
			}
		}

		// KeyCodes V1 mapping reference (Apple Script):
		// https://eastmanreference.com/complete-list-of-applescript-key-codes

		static const EKeyCode sAppleKeyCodeMap_V1_00_7F[] =
		{
			/* 0x00  0 */ EKeyCode::CharA,
			/* 0x01  1 */ EKeyCode::CharS,
			/* 0x02  2 */ EKeyCode::CharD,
			/* 0x03  3 */ EKeyCode::CharF,
			/* 0x04  4 */ EKeyCode::CharH,
			/* 0x05  5 */ EKeyCode::CharG,
			/* 0x06  6 */ EKeyCode::CharZ,
			/* 0x07  7 */ EKeyCode::CharX,
			/* 0x08  8 */ EKeyCode::CharC,
			/* 0x09  9 */ EKeyCode::CharV,
			/* 0x0A 10 */ EKeyCode::Unknown,
			/* 0x0B 11 */ EKeyCode::CharB,
			/* 0x0C 12 */ EKeyCode::CharQ,
			/* 0x0D 13 */ EKeyCode::CharW,
			/* 0x0E 14 */ EKeyCode::CharE,
			/* 0x0F 15 */ EKeyCode::CharR,
			/* 0x10 16 */ EKeyCode::CharY,
			/* 0x11 17 */ EKeyCode::CharT,
			/* 0x12 18 */ EKeyCode::Num1,
			/* 0x13 19 */ EKeyCode::Num2,
			/* 0x14 20 */ EKeyCode::Num3,
			/* 0x15 21 */ EKeyCode::Num4,
			/* 0x16 22 */ EKeyCode::Num6,
			/* 0x17 23 */ EKeyCode::Num5,
			/* 0x18 24 */ EKeyCode::Unknown,
			/* 0x19 25 */ EKeyCode::Num9,
			/* 0x1A 26 */ EKeyCode::Num7,
			/* 0x1B 27 */ EKeyCode::Unknown,
			/* 0x1C 28 */ EKeyCode::Num8,
			/* 0x1D 29 */ EKeyCode::Num0,
			/* 0x1E 30 */ EKeyCode::Unknown,
			/* 0x1F 31 */ EKeyCode::CharO,
			/* 0x20 32 */ EKeyCode::CharU,
			/* 0x21 33 */ EKeyCode::Unknown,
			/* 0x22 34 */ EKeyCode::CharI,
			/* 0x23 35 */ EKeyCode::CharP,
			/* 0x24 36 */ EKeyCode::Enter,
			/* 0x25 37 */ EKeyCode::CharL,
			/* 0x26 38 */ EKeyCode::CharJ,
			/* 0x27 39 */ EKeyCode::Unknown,
			/* 0x28 40 */ EKeyCode::CharK,
			/* 0x29 41 */ EKeyCode::Unknown,
			/* 0x2A 42 */ EKeyCode::Unknown,
			/* 0x2B 43 */ EKeyCode::Unknown,
			/* 0x2C 44 */ EKeyCode::Unknown,
			/* 0x2D 45 */ EKeyCode::CharN,
			/* 0x2E 46 */ EKeyCode::CharM,
			/* 0x2F 47 */ EKeyCode::Unknown,
			/* 0x30 48 */ EKeyCode::Tab,
			/* 0x31 49 */ EKeyCode::Space,
			/* 0x32 50 */ EKeyCode::Unknown,
			/* 0x33 51 */ EKeyCode::Unknown,
			/* 0x34 52 */ EKeyCode::Unknown,
			/* 0x35 53 */ EKeyCode::Escape,
			/* 0x36 54 */ EKeyCode::Unknown,
			/* 0x37 55 */ EKeyCode::Unknown,
			/* 0x38 56 */ EKeyCode::Unknown,
			/* 0x39 57 */ EKeyCode::Unknown,
			/* 0x3A 58 */ EKeyCode::Unknown,
			/* 0x3B 59 */ EKeyCode::Unknown,
			/* 0x3C 60 */ EKeyCode::Unknown,
			/* 0x3D 61 */ EKeyCode::Unknown,
			/* 0x3E 62 */ EKeyCode::Unknown,
			/* 0x3F 63 */ EKeyCode::Unknown,
			/* 0x40 64 */ EKeyCode::Unknown,
			/* 0x41 65 */ EKeyCode::Unknown,
			/* 0x42 66 */ EKeyCode::Unknown,
			/* 0x43 67 */ EKeyCode::Unknown,
			/* 0x44 68 */ EKeyCode::Unknown,
			/* 0x45 69 */ EKeyCode::Unknown,
			/* 0x46 70 */ EKeyCode::Unknown,
			/* 0x47 71 */ EKeyCode::Unknown,
			/* 0x48 72 */ EKeyCode::Unknown,
			/* 0x49 73 */ EKeyCode::Unknown,
			/* 0x4A 74 */ EKeyCode::Unknown,
			/* 0x4B 75 */ EKeyCode::Unknown,
			/* 0x4C 76 */ EKeyCode::Unknown,
			/* 0x4D 77 */ EKeyCode::Unknown,
			/* 0x4E 78 */ EKeyCode::Unknown,
			/* 0x4F 79 */ EKeyCode::Unknown,
			/* 0x50 80 */ EKeyCode::Unknown,
			/* 0x51 81 */ EKeyCode::Unknown,
			/* 0x52 82 */ EKeyCode::Unknown,
			/* 0x53 83 */ EKeyCode::Unknown,
			/* 0x54 84 */ EKeyCode::Unknown,
			/* 0x55 85 */ EKeyCode::Unknown,
			/* 0x56 86 */ EKeyCode::Unknown,
			/* 0x57 87 */ EKeyCode::Unknown,
			/* 0x58 88 */ EKeyCode::Unknown,
			/* 0x59 89 */ EKeyCode::Unknown,
			/* 0x5A 90 */ EKeyCode::Unknown,
			/* 0x5B 91 */ EKeyCode::Unknown,
			/* 0x5C 92 */ EKeyCode::Unknown,
			/* 0x5D 93 */ EKeyCode::Unknown,
			/* 0x5E 94 */ EKeyCode::Unknown,
			/* 0x5F 95 */ EKeyCode::Unknown,
			/* 0x60 96 */ EKeyCode::F5,
			/* 0x61 97 */ EKeyCode::F6,
			/* 0x62 98 */ EKeyCode::F7,
			/* 0x63 99 */ EKeyCode::F3,
			/* 0x64 100 */ EKeyCode::F8,
			/* 0x65 101 */ EKeyCode::F9,
			/* 0x66 102 */ EKeyCode::Unknown,
			/* 0x67 103 */ EKeyCode::F11,
			/* 0x68 104 */ EKeyCode::Unknown,
			/* 0x69 105 */ EKeyCode::Unknown,
			/* 0x6A 106 */ EKeyCode::Unknown,
			/* 0x6B 107 */ EKeyCode::Unknown,
			/* 0x6C 108 */ EKeyCode::Unknown,
			/* 0x6D 109 */ EKeyCode::F10,
			/* 0x6E 110 */ EKeyCode::Unknown,
			/* 0x6F 111 */ EKeyCode::F12,
			/* 0x70 112 */ EKeyCode::Unknown,
			/* 0x71 113 */ EKeyCode::Unknown,
			/* 0x72 114 */ EKeyCode::Insert,
			/* 0x73 115 */ EKeyCode::Home,
			/* 0x74 116 */ EKeyCode::PageUp,
			/* 0x75 117 */ EKeyCode::Delete,
			/* 0x76 118 */ EKeyCode::F4,
			/* 0x77 119 */ EKeyCode::End,
			/* 0x78 120 */ EKeyCode::F2,
			/* 0x79 121 */ EKeyCode::PageDown,
			/* 0x7A 122 */ EKeyCode::F1,
			/* 0x7B 123 */ EKeyCode::ArrowLeft,
			/* 0x7C 124 */ EKeyCode::ArrowRight,
			/* 0x7D 125 */ EKeyCode::ArrowDown,
			/* 0x7E 126 */ EKeyCode::ArrowUp,
			/* 0x7F 127 */ EKeyCode::Unknown,
		};

		EKeyCode _osxGetSysKeyCodeV1( unsigned short pNSKeyCode )
		{
			if( ( pNSKeyCode >= 0x00 ) && ( pNSKeyCode <= 0x7F ) )
			{
				const auto baseIndex = pNSKeyCode;
				return sAppleKeyCodeMap_V1_00_7F[baseIndex];
			}

			return EKeyCode::Unknown;
		}

		// KeyCodes V2 mapping reference (USB-compliant):
		// https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
		// https://developer.apple.com/library/archive/technotes/tn2450/_index.html

		static const EKeyCode sAppleKeyCodeMap_V2_04_73[] =
		{
			/* 0x04 */ EKeyCode::CharA,
			/* 0x05 */ EKeyCode::CharB,
			/* 0x06 */ EKeyCode::CharC,
			/* 0x07 */ EKeyCode::CharD,
			/* 0x08 */ EKeyCode::CharE,
			/* 0x09 */ EKeyCode::CharF,
			/* 0x0A */ EKeyCode::CharG,
			/* 0x0B */ EKeyCode::CharH,
			/* 0x0C */ EKeyCode::CharI,
			/* 0x0D */ EKeyCode::CharJ,
			/* 0x0E */ EKeyCode::CharK,
			/* 0x0F */ EKeyCode::CharL,
			/* 0x10 */ EKeyCode::CharM,
			/* 0x11 */ EKeyCode::CharN,
			/* 0x12 */ EKeyCode::CharO,
			/* 0x13 */ EKeyCode::CharP,
			/* 0x14 */ EKeyCode::CharQ,
			/* 0x15 */ EKeyCode::CharR,
			/* 0x16 */ EKeyCode::CharS,
			/* 0x17 */ EKeyCode::CharT,
			/* 0x18 */ EKeyCode::CharU,
			/* 0x19 */ EKeyCode::CharV,
			/* 0x1A */ EKeyCode::CharW,
			/* 0x1B */ EKeyCode::CharX,
			/* 0x1C */ EKeyCode::CharY,
			/* 0x1D */ EKeyCode::CharZ,
            /* 0x1E */ EKeyCode::Num0,
			/* 0x1F */ EKeyCode::Num1,
			/* 0x20 */ EKeyCode::Num2,
			/* 0x21 */ EKeyCode::Num3,
			/* 0x22 */ EKeyCode::Num4,
			/* 0x23 */ EKeyCode::Num5,
			/* 0x24 */ EKeyCode::Num6,
			/* 0x25 */ EKeyCode::Num7,
			/* 0x26 */ EKeyCode::Num8,
			/* 0x27 */ EKeyCode::Num9,
			/* 0x28 */ EKeyCode::Enter,
			/* 0x29 */ EKeyCode::Escape,
			/* 0x2A */ EKeyCode::Backspace,
			/* 0x2B */ EKeyCode::Tab,
			/* 0x2C */ EKeyCode::Space,
			/* 0x2D */ /* - and _        */ EKeyCode::Unknown,
			/* 0x2E */ /* = and +        */ EKeyCode::Unknown,
			/* 0x2F */ /* [ and {        */ EKeyCode::Unknown,
			/* 0x30 */ /* ] and }        */ EKeyCode::Unknown,
			/* 0x31 */ /* \ and |        */ EKeyCode::Unknown,
			/* 0x32 */ /* Non-US # and ~ */ EKeyCode::Unknown,
			/* 0x33 */ /* ; and :        */ EKeyCode::Unknown,
			/* 0x34 */ /* ' and "        */ EKeyCode::Unknown,
			/* 0x35 */ /*   Tilde        */ EKeyCode::Unknown,
			/* 0x36 */ /* , and "<"      */ EKeyCode::Unknown,
			/* 0x37 */ /* . and ">"      */ EKeyCode::Unknown,
			/* 0x38 */ /* / and ?        */ EKeyCode::Unknown,
			/* 0x39 */ EKeyCode::CapsLock,
			/* 0x3A */ EKeyCode::F1,
			/* 0x3B */ EKeyCode::F2,
			/* 0x3C */ EKeyCode::F3,
			/* 0x3D */ EKeyCode::F4,
			/* 0x3E */ EKeyCode::F5,
			/* 0x3F */ EKeyCode::F6,
			/* 0x40 */ EKeyCode::F7,
			/* 0x41 */ EKeyCode::F8,
			/* 0x42 */ EKeyCode::F9,
			/* 0x43 */ EKeyCode::F10,
			/* 0x44 */ EKeyCode::F11,
			/* 0x45 */ EKeyCode::F12,
			/* 0x46 */ /* Print Screen   */ EKeyCode::Unknown,
			/* 0x47 */ /* Scroll Lock    */ EKeyCode::Unknown,
			/* 0x48 */ /* Pause          */ EKeyCode::Unknown,
			/* 0x49 */ EKeyCode::Insert,
			/* 0x4A */ EKeyCode::Home,
			/* 0x4B */ EKeyCode::PageUp,
			/* 0x4C */ EKeyCode::Delete,
			/* 0x4D */ EKeyCode::End,
			/* 0x4E */ EKeyCode::PageDown,
			/* 0x4F */ EKeyCode::ArrowRight,
			/* 0x50 */ EKeyCode::ArrowLeft,
			/* 0x51 */ EKeyCode::ArrowDown,
			/* 0x52 */ EKeyCode::ArrowUp,
			/* 0x53 */ /* Keypad Num Lock */ EKeyCode::Unknown,
			/* 0x54 */ EKeyCode::NumPadDiv,
			/* 0x55 */ EKeyCode::NumPadMul,
			/* 0x56 */ EKeyCode::NumPadSub,
			/* 0x57 */ EKeyCode::NumPadAdd,
			/* 0x58 */ EKeyCode::Enter,
			/* 0x59 */ EKeyCode::NumPad0,
			/* 0x5A */ EKeyCode::NumPad1,
			/* 0x5B */ EKeyCode::NumPad2,
			/* 0x5C */ EKeyCode::NumPad3,
			/* 0x5D */ EKeyCode::NumPad4,
			/* 0x5E */ EKeyCode::NumPad5,
			/* 0x5F */ EKeyCode::NumPad6,
			/* 0x60 */ EKeyCode::NumPad7,
			/* 0x61 */ EKeyCode::NumPad8,
			/* 0x62 */ EKeyCode::NumPad9,
			/* 0x63 */ EKeyCode::NumPadDot,
			/* 0x64 */ /* \ and | */ EKeyCode::Unknown,
		};

		static const EKeyCode sAppleKeyCodeMap_V2_E0_E7[] =
		{
			/* 0xE0 */ EKeyCode::CtrlLeft,
			/* 0xE1 */ EKeyCode::ShiftLeft,
			/* 0xE2 */ EKeyCode::AltLeft,
			/* 0xE3 */ /* Keyboard Left GUI       */ EKeyCode::Unknown,
			/* 0xE4 */ EKeyCode::CtrlRight,
			/* 0xE5 */ EKeyCode::ShiftRight,
			/* 0xE6 */ EKeyCode::AltRight,
			/* 0xE7 */ /* Keyboard Right GUI      */ EKeyCode::Unknown,
		};

		EKeyCode _osxGetSysKeyCodeV2( unsigned short pNSKeyCode )
		{
			if( ( pNSKeyCode >= 0x04 ) && ( pNSKeyCode <= 0x73 ) )
			{
				const auto baseIndex = pNSKeyCode - 0x04;
				return sAppleKeyCodeMap_V2_04_73[baseIndex];
			}

			if( ( pNSKeyCode >= 0xE0 ) && ( pNSKeyCode <= 0xE7 ) )
			{
				const auto baseIndex = pNSKeyCode - 0xE0;
				return sAppleKeyCodeMap_V2_E0_E7[baseIndex];
			}

			return EKeyCode::Unknown;
		}

	}
	
}
