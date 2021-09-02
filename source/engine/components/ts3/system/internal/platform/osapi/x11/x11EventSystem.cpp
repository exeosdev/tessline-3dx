
#include <ts3/system/eventCore.h>
#include <ts3/math/vectorOps.h>
#include <X11/keysym.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3
{

	bool _x11EventPeek( ContextNativeData & pSCNativeData, XEvent & pEvent );
	bool _x11EventWait( ContextNativeData & pSCNativeData, XEvent & pEvent );
	bool _x11TranslateAndDispatch( EventController & pEventController, XEvent & pXEvent );
	bool _x11TranslateGenericEvent( EventDispatcher & pDispatcher, XEvent & pXEvent, Event & pEvent );
	bool _x11TranslateInputEvent( EventInputState & pInputState, XEvent & pXEvent, Event & pEvent );
	bool _x11TranslateInputEventMouseButton( EventInputState & pInputState, XEvent & pXEvent, Event & pEvent, MouseButtonActionType pButtonAction );
	bool _x11TranslatetemEvent( EventDispatcher & pDispatcher, XEvent & pXEvent, Event & pEvent );
	KeyCode _x11GettemKeyCode( KeySym pXkeySym );
	X11MouseButtonID _x11GetMouseButtonID( uint32 pButton );
	Bitmask<MouseButtonFlagBits> _x11GetMouseButtonStateMask( uint32 pState );


	void EventController::_sysInitialize()
	{}

	void EventController::_sysRelease()
	{}

	void EventController::_sysAddEventSource( EventSource & pEventSource )
	{}

	void EventController::_sysRemoveEventSource( EventSource & pEventSource )
	{}

	void EventController::_sysDispatchNextEvent()
	{
		XEvent xevent;
		if( _x11EventPeek( mContext->mNativeData, xevent ) )
		{
			_x11TranslateAndDispatch( *this, xevent );
		}
	}

	void EventController::_sysDispatchNextEventWait()
	{
		XEvent xevent;
		if( _x11EventWait( mContext->mNativeData, xevent ) )
		{
			_x11TranslateAndDispatch( *this, xevent );
		}
	}

	void EventController::_sysDispatchQueuedEvents()
	{
		XEvent xevent;
		while( _x11EventPeek( mContext->mNativeData, xevent ) )
		{
			_x11TranslateAndDispatch( *this, xevent );
		}
	}

	void EventController::_sysDispatchQueuedEventsWait()
	{
		uint32 fetchedEventsNum = 0;

		XEvent xevent;
		while( _x11EventPeek( mContext->mNativeData, xevent ) )
		{
			_x11TranslateAndDispatch( *this, xevent );
			++fetchedEventsNum;
		}

		if( ( fetchedEventsNum == 0 ) && _x11EventWait( mContext->mNativeData, xevent ) )
		{
			_x11TranslateAndDispatch( *this, xevent );
		}
	}



	bool _x11EventPeek( ContextNativeData & pSCNativeData, XEvent & pEvent )
	{
		if( XEventsQueued( pSCNativeData.display, QueuedAfterReading ) > 0 )
		{
			XNextEvent( pSCNativeData.display, &pEvent );
			return true;
		}

		return false;
	}

	bool _x11EventWait( ContextNativeData & pSCNativeData, XEvent & pEvent )
	{
		if( XEventsQueued( pSCNativeData.display, QueuedAfterFlush ) > 0 )
		{
			XNextEvent( pSCNativeData.display, &pEvent );
			return true;
		}

		return false;
	}

	bool _x11TranslateAndDispatch( EventController & pEventController, XEvent & pXEvent )
	{
		auto & dispatcher = pEventController.getActiveDispatcher();

		Event sysEvent;

		if( ( pXEvent.type >= KeyPress ) && ( pXEvent.type <= MotionNotify ) )
		{
			auto & inputState = dispatcher.getInputState();
			if( !_x11TranslateInputEvent( inputState, pXEvent, sysEvent ) )
			{
				return false;
			}
		}
		else if( ( pXEvent.type >= EnterNotify ) && ( pXEvent.type <= MapRequest ) )
		{
			if( !_x11TranslateGenericEvent( dispatcher, pXEvent, sysEvent ) )
			{
				return false;
			}
		}
		else
		{
			if( !_x11TranslatetemEvent( dispatcher, pXEvent, sysEvent ) )
			{
				return false;
			}
		}

		pEventController.dispatchEvent( sysEvent );

		return true;
	}

	bool _x11TranslateGenericEvent( EventDispatcher & pDispatcher, XEvent & pXEvent, Event & pEvent )
	{
		switch( pXEvent.type )
		{
			case EnterNotify:
				break;
			case LeaveNotify:
				break;
			case FocusIn:
				break;
			case FocusOut:
				break;
			case KeymapNotify:
				break;
			case Expose:
				break;
			case GraphicsExpose:
				break;
			case NoExpose:
				break;
			case VisibilityNotify:
				break;
			case CreateNotify:
				break;
			case DestroyNotify:
				break;
			case UnmapNotify:
				break;
			case MapNotify:
				break;
			case MapRequest:
				break;
			default:
				return false;
		}

		return true;
	}

	bool _x11TranslateInputEvent( EventInputState & pInputState, XEvent & pXEvent, Event & pEvent )
	{
		switch( pXEvent.type )
		{
			// Note on the keysym index:
			// The standard rules for obtaining a KeySym from a KeyPress event make use of only the Group 1
			// and Group 2 KeySyms, no interpretation of other KeySyms in the list is given. Which group to
			// use is determined by the modifier state.
			// [Source: https://tronche.com/gui/x/xlib/input/keyboard-encoding.html#KeySym]
			case KeyPress:
			{
				auto keysym = XLookupKeysym( &( pXEvent.xkey ), 0 );
				auto keycode = _x11GettemKeyCode( keysym );
				auto & eventData = pEvent.eInputKeyboardKey;
				eventData.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
				eventData.keyboardState = &( pInputState.keyboardState );
				eventData.keyCode = keycode;
				eventData.keyAction = KeyActionType::Press;
				break;
			}
			case KeyRelease:
			{
				auto keysym = XLookupKeysym( &( pXEvent.xkey ), 0 );
				auto keycode = _x11GettemKeyCode( keysym );
				auto & eventData = pEvent.eInputKeyboardKey;
				eventData.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
				eventData.keyboardState = &( pInputState.keyboardState );
				eventData.keyCode = keycode;
				eventData.keyAction = KeyActionType::Release;

				break;
			}
			case ButtonPress:
			{
				_x11TranslateInputEventMouseButton( pInputState, pXEvent, pEvent, MouseButtonActionType::Click );

				break;
			}
			case ButtonRelease:
			{
				_x11TranslateInputEventMouseButton( pInputState, pXEvent, pEvent, MouseButtonActionType::Release );

				break;
			}
			case MotionNotify:
			{
				const math::Vec2i32 cursorPos { pXEvent.xmotion.x, pXEvent.xmotion.y };

				if ( pInputState.mouseLastRegPos == CX_EVENT_MOUDE_POINT_INVALID )
				{
					pInputState.mouseLastRegPos = cursorPos;
				}

				auto & eInputMouseMove = pEvent.eInputMouseMove;
				eInputMouseMove.eventCode = E_EVENT_CODE_INPUT_MOUSE_MOVE;
				eInputMouseMove.cursorPos = cursorPos;

				// Old method, based on the state mask provided by the system. Although *some* modifiers are there,
				// extra mouse buttons (X1 and X2) are not reported. Since there is our own internal state mask kept
				// inside the EventInputState object, we just use that. This line is supposed to be kept here just
				// for the future reference or if additional modifiers need to be supported.
				// eInputMouseMove.buttonStateMask = _x11GetMouseButtonStateMask( pXEvent.xmotion.state );

				eInputMouseMove.buttonStateMask = pInputState.mouseButtonStateMask;
				eInputMouseMove.movementDelta = cursorPos - pInputState.mouseLastRegPos;
				pInputState.mouseLastRegPos = cursorPos;

				break;
			}
			default:
			{
				return false;
			}
		}

		return pEvent.commonData.eventCode != E_EVENT_CODE_UNDEFINED;
	}

	bool _x11TranslateInputEventMouseButton( EventInputState & pInputState, XEvent & pXEvent, Event & pEvent, MouseButtonActionType pButtonAction )
	{
		const math::Vec2i32 cursorPos { pXEvent.xbutton.x, pXEvent.xbutton.y };

		auto X11MouseButtonID = _x11GetMouseButtonID( pXEvent.xbutton.button );

		switch( X11MouseButtonID )
		{
			case X11MouseButtonID::Left:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = MouseButtonID::Left;

				if( pButtonAction == MouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_LEFT_BIT );
				}
				else if( pButtonAction == MouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_LEFT_BIT );
				}

				break;
			}
			case X11MouseButtonID::Middle:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = MouseButtonID::Middle;

				if( pButtonAction == MouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT );
				}
				else if( pButtonAction == MouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT );
				}

				break;
			}
			case X11MouseButtonID::Right:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = MouseButtonID::Right;

				if( pButtonAction == MouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT );
				}
				else if( pButtonAction == MouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT );
				}

				break;
			}
			case X11MouseButtonID::VWheelUp:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = 0;
				eInputMouseScroll.scrollDelta.y = 100;
				break;
			}
			case X11MouseButtonID::VWheelDown:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = 0;
				eInputMouseScroll.scrollDelta.y = -100;

				break;
			}
			case X11MouseButtonID::HWheelLeft:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = -100;
				eInputMouseScroll.scrollDelta.y = 0;

				break;
			}
			case X11MouseButtonID::HWheelRight:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = 100;
				eInputMouseScroll.scrollDelta.y = 0;

				break;
			}
			case X11MouseButtonID::Xbtn1:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = MouseButtonID::XB1;

				if( pButtonAction == MouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_XB1_BIT );
				}
				else if( pButtonAction == MouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_XB1_BIT );
				}

				break;
			}
			case X11MouseButtonID::Xbtn2:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = MouseButtonID::XB2;

				if( pButtonAction == MouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_XB2_BIT );
				}
				else if( pButtonAction == MouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_XB2_BIT );
				}

				break;
			}
			default:
			{
				return false;
			}
		}

		return true;
	}

	bool _x11TranslatetemEvent( EventDispatcher & pDispatcher, XEvent & pXEvent, Event & pEvent )
	{
		switch( pXEvent.type )
		{
			case ClientMessage:
			{
				auto & scNativeData = pDispatcher.mContext->mNativeData;
				// Type of wm protocol message is stored in data.l[0].
				long wmpMessageType = pXEvent.xclient.data.l[0];
				if ( wmpMessageType == scNativeData.wmpDeleteWindow )
				{
					auto & eventData = pEvent.eWindowUpdateClose;
					eventData.eventCode = E_EVENT_CODE_WINDOW_UPDATE_CLOSE;
				}
				break;
			}
			default:
			{
				return false;
			}
		}

		return true;
	}


	static const KeyCode x11KeycodeMap_20_7A[] =
	{
		/* 0x0020 */ KeyCode::Space,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x0030 */ KeyCode::Num0,
		/* 0x0031 */ KeyCode::Num1,
		/* 0x0032 */ KeyCode::Num2,
		/* 0x0033 */ KeyCode::Num3,
		/* 0x0034 */ KeyCode::Num4,
		/* 0x0035 */ KeyCode::Num5,
		/* 0x0036 */ KeyCode::Num6,
		/* 0x0037 */ KeyCode::Num7,
		/* 0x0038 */ KeyCode::Num8,
		/* 0x0039 */ KeyCode::Num9,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x0041 */ KeyCode::CharA,
		/* 0x0042 */ KeyCode::CharB,
		/* 0x0043 */ KeyCode::CharC,
		/* 0x0044 */ KeyCode::CharD,
		/* 0x0045 */ KeyCode::CharE,
		/* 0x0046 */ KeyCode::CharF,
		/* 0x0047 */ KeyCode::CharG,
		/* 0x0048 */ KeyCode::CharH,
		/* 0x0049 */ KeyCode::CharI,
		/* 0x004a */ KeyCode::CharJ,
		/* 0x004b */ KeyCode::CharK,
		/* 0x004c */ KeyCode::CharL,
		/* 0x004d */ KeyCode::CharM,
		/* 0x004e */ KeyCode::CharN,
		/* 0x004f */ KeyCode::CharO,
		/* 0x0050 */ KeyCode::CharP,
		/* 0x0051 */ KeyCode::CharQ,
		/* 0x0052 */ KeyCode::CharR,
		/* 0x0053 */ KeyCode::CharS,
		/* 0x0054 */ KeyCode::CharT,
		/* 0x0055 */ KeyCode::CharU,
		/* 0x0056 */ KeyCode::CharV,
		/* 0x0057 */ KeyCode::CharW,
		/* 0x0058 */ KeyCode::CharX,
		/* 0x0059 */ KeyCode::CharY,
		/* 0x005a */ KeyCode::CharZ,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x0061 */ KeyCode::CharA,
		/* 0x0062 */ KeyCode::CharB,
		/* 0x0063 */ KeyCode::CharC,
		/* 0x0064 */ KeyCode::CharD,
		/* 0x0065 */ KeyCode::CharE,
		/* 0x0066 */ KeyCode::CharF,
		/* 0x0067 */ KeyCode::CharG,
		/* 0x0068 */ KeyCode::CharH,
		/* 0x0069 */ KeyCode::CharI,
		/* 0x006a */ KeyCode::CharJ,
		/* 0x006b */ KeyCode::CharK,
		/* 0x006c */ KeyCode::CharL,
		/* 0x006d */ KeyCode::CharM,
		/* 0x006e */ KeyCode::CharN,
		/* 0x006f */ KeyCode::CharO,
		/* 0x0070 */ KeyCode::CharP,
		/* 0x0071 */ KeyCode::CharQ,
		/* 0x0072 */ KeyCode::CharR,
		/* 0x0073 */ KeyCode::CharS,
		/* 0x0074 */ KeyCode::CharT,
		/* 0x0075 */ KeyCode::CharU,
		/* 0x0076 */ KeyCode::CharV,
		/* 0x0077 */ KeyCode::CharW,
		/* 0x0078 */ KeyCode::CharX,
		/* 0x0079 */ KeyCode::CharY,
		/* 0x007a */ KeyCode::CharZ,
	};

	static const KeyCode x11KeycodeMap_50_57[] =
	{
		/* 0xff50 */ KeyCode::Home,
		/* 0xff51 */ KeyCode::ArrowLeft,
		/* 0xff52 */ KeyCode::ArrowUp,
		/* 0xff53 */ KeyCode::ArrowRight,
		/* 0xff54 */ KeyCode::ArrowDown,
		/* 0xff55 */ KeyCode::PageUp,
		/* 0xff56 */ KeyCode::PageDown,
		/* 0xff57 */ KeyCode::End
	};

	static const KeyCode x11KeycodeMap_AA_B9[] =
	{
		/* 0xffaa */ KeyCode::NumpadMul,
		/* 0xffab */ KeyCode::NumpadAdd,
		/* 0xffac */ KeyCode::Unknown,
		/* 0xffad */ KeyCode::NumpadSub,
		/* 0xffae */ KeyCode::NumpadDot,
		/* 0xffaf */ KeyCode::NumpadDiv,
		/* 0xffb0 */ KeyCode::Numpad0,
		/* 0xffb1 */ KeyCode::Numpad1,
		/* 0xffb2 */ KeyCode::Numpad2,
		/* 0xffb3 */ KeyCode::Numpad3,
		/* 0xffb4 */ KeyCode::Numpad4,
		/* 0xffb5 */ KeyCode::Numpad5,
		/* 0xffb6 */ KeyCode::Numpad6,
		/* 0xffb7 */ KeyCode::Numpad7,
		/* 0xffb8 */ KeyCode::Numpad8,
		/* 0xffb9 */ KeyCode::Numpad9,
	};

	static const KeyCode x11KeycodeMap_BE_C9[] =
	{
		/* 0xffbe */ KeyCode::F1,
		/* 0xffbf */ KeyCode::F2,
		/* 0xffc0 */ KeyCode::F3,
		/* 0xffc1 */ KeyCode::F4,
		/* 0xffc2 */ KeyCode::F5,
		/* 0xffc3 */ KeyCode::F6,
		/* 0xffc4 */ KeyCode::F7,
		/* 0xffc5 */ KeyCode::F8,
		/* 0xffc6 */ KeyCode::F9,
		/* 0xffc7 */ KeyCode::F10,
		/* 0xffc8 */ KeyCode::F11,
		/* 0xffc9 */ KeyCode::F12
	};

	static const KeyCode x11KeycodeMap_E1_EA[] =
	{
		/* 0xffe1 */ KeyCode::ShiftLeft,
		/* 0xffe2 */ KeyCode::ShiftRight,
		/* 0xffe3 */ KeyCode::CtrlLeft,
		/* 0xffe4 */ KeyCode::CtrlRight,
		/* 0xffe5 */ KeyCode::CapsLock,
		/* 0xffe6 */ KeyCode::Unknown,
		/* 0xffe7 */ KeyCode::Unknown,
		/* 0xffe8 */ KeyCode::Unknown,
		/* 0xffe9 */ KeyCode::AltLeft,
		/* 0xffea */ KeyCode::AltRight
	};


	KeyCode _x11GettemKeyCode( KeySym pXkeySym )
	{
		if ( ( pXkeySym >= 0x0020 ) && ( pXkeySym <= 0x007a ) )
		{
			auto baseIndex = static_cast<uint32>( pXkeySym ) - 0x0020;
			return x11KeycodeMap_20_7A[baseIndex];
		}

		if ( ( pXkeySym >= 0xff50 ) && ( pXkeySym <= 0xffea ) )
		{
			if ( ( pXkeySym >= 0xff50 ) && ( pXkeySym <= 0xff57 ) )
			{
				auto baseIndex = static_cast<uint32>( pXkeySym ) - 0xff50;
				return x11KeycodeMap_50_57[baseIndex];
			}

			if ( ( pXkeySym >= 0xffaa ) && ( pXkeySym <= 0xffb9 ) )
			{
				auto baseIndex = static_cast<uint32>( pXkeySym ) - 0xffaa;
				return x11KeycodeMap_AA_B9[baseIndex];
			}

			if ( ( pXkeySym >= 0xffbe ) && ( pXkeySym <= 0xffc9 ) )
			{
				auto baseIndex = static_cast<uint32>( pXkeySym ) - 0xffbe;
				return x11KeycodeMap_BE_C9[baseIndex];
			}

			if ( ( pXkeySym >= 0xffe1 ) && ( pXkeySym <= 0xffea ) )
			{
				auto baseIndex = static_cast<uint32>( pXkeySym ) - 0xffe1;
				return x11KeycodeMap_E1_EA[baseIndex];
			}
		}

		if ( pXkeySym == XK_Tab )
		{
			return KeyCode::Tab;
		}
		else if ( pXkeySym == XK_Return )
		{
			return KeyCode::Enter;
		}
		else if ( pXkeySym == XK_Escape )
		{
			return KeyCode::Escape;
		}
		else if ( pXkeySym == XK_BackSpace )
		{
			return KeyCode::Backspace;
		}
		else if ( pXkeySym == XK_Insert )
		{
			return KeyCode::Insert;
		}
		else if ( pXkeySym == XK_Delete )
		{
			return KeyCode::Delete;
		}

		return KeyCode::Unknown;
	}

	static constexpr X11MouseButtonID x11MouseButtonIDMap[] =
	{
		X11MouseButtonID::Left,
		X11MouseButtonID::Middle,
		X11MouseButtonID::Right,
		X11MouseButtonID::VWheelUp,
		X11MouseButtonID::VWheelDown,
		X11MouseButtonID::HWheelLeft,
		X11MouseButtonID::HWheelRight,
		X11MouseButtonID::Xbtn1,
		X11MouseButtonID::Xbtn2,
		X11MouseButtonID::Unknown
	};
	static constexpr auto x11MouseButtonMaxIndex = staticArraySize( x11MouseButtonIDMap ) - 1;

	X11MouseButtonID _x11GetMouseButtonID( uint32 pButton )
	{
		auto mouseButtonIndex = static_cast<size_t>( pButton - 1 );
		mouseButtonIndex = getMinOf( mouseButtonIndex, x11MouseButtonMaxIndex );
		return x11MouseButtonIDMap[mouseButtonIndex];
	}

	Bitmask<MouseButtonFlagBits> _x11GetMouseButtonStateMask( uint32 pState )
	{
		Bitmask<uint32> inputStateMask = pState;
		Bitmask<MouseButtonFlagBits> buttonStateMask;
		if( pState != 0 )
		{
			if( inputStateMask.isSetAnyOf( Button1Mask | Button1MotionMask ) )
			{
				inputStateMask.unset( Button1Mask | Button1MotionMask );
				buttonStateMask.set( SYS_MOUSE_BUTTON_FLAG_LEFT_BIT );
			}
			if( inputStateMask.isSetAnyOf( Button2Mask | Button2MotionMask ) )
			{
				inputStateMask.unset( Button2Mask | Button2MotionMask );
				buttonStateMask.set( SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT );
			}
			if( inputStateMask.isSetAnyOf( Button3Mask | Button3MotionMask ) )
			{
				inputStateMask.unset( Button3Mask | Button3MotionMask );
				buttonStateMask.set( SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT);
			}
		}
		return buttonStateMask;
	}

}
#endif
