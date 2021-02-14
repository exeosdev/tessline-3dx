
#include <ts3/system/eventCoreNative.h>
#include <ts3/math/vectorOps.h>
#include <X11/keysym.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3
{

	bool _x11EventPeek( SysContextNativeData & pSCNativeData, XEvent & pEvent );
	bool _x11EventWait( SysContextNativeData & pSCNativeData, XEvent & pEvent );
	bool _x11TranslateAndDispatch( SysEventController & pEventController, XEvent & pXEvent );
	bool _x11TranslateGenericEvent( SysEventDispatcher & pDispatcher, XEvent & pXEvent, SysEvent & pEvent );
	bool _x11TranslateInputEvent( SysEventInputState & pInputState, XEvent & pXEvent, SysEvent & pEvent );
	bool _x11TranslateInputEventMouseButton( SysEventInputState & pInputState, XEvent & pXEvent, SysEvent & pEvent, SysMouseButtonActionType pButtonAction );
	bool _x11TranslateSystemEvent( SysEventDispatcher & pDispatcher, XEvent & pXEvent, SysEvent & pEvent );
	SysKeyCode _x11GetSystemKeyCode( KeySym pXkeySym );
	SysX11MouseButtonID _x11GetMouseButtonID( uint32 pButton );
	Bitmask<SysMouseButtonFlagBits> _x11GetMouseButtonStateMask( uint32 pState );


	void SysEventController::_sysInitialize()
	{}

	void SysEventController::_sysRelease()
	{}

	void SysEventController::_sysAddEventSource( SysEventSource & pEventSource )
	{}

	void SysEventController::_sysRemoveEventSource( SysEventSource & pEventSource )
	{}

	void SysEventController::_sysDispatchNextEvent()
	{
		XEvent xevent;
		if( _x11EventPeek( pEventController.systemContext->nativeData, xevent ) )
		{
			_x11TranslateAndDispatch( pEventController, xevent );
		}
	}

	void SysEventController::_sysDispatchNextEventWait()
	{
		XEvent xevent;
		if( _x11EventWait( pEventController.systemContext->nativeData, xevent ) )
		{
			_x11TranslateAndDispatch( pEventController, xevent );
		}
	}

	void SysEventController::_sysDispatchQueuedEvents()
	{
		XEvent xevent;
		while( _x11EventPeek( pEventController.systemContext->nativeData, xevent ) )
		{
			_x11TranslateAndDispatch( pEventController, xevent );
		}
	}

	void SysEventController::_sysDispatchQueuedEventsWait()
	{
		uint32 fetchedEventsNum = 0;

		XEvent xevent;
		while( _x11EventPeek( pEventController.systemContext->nativeData, xevent ) )
		{
			_x11TranslateAndDispatch( pEventController, xevent );
			++fetchedEventsNum;
		}

		if( ( fetchedEventsNum == 0 ) && _x11EventWait( pEventController.systemContext->nativeData, xevent ) )
		{
			_x11TranslateAndDispatch( pEventController, xevent );
		}
	}



	bool _x11EventPeek( SysContextNativeData & pSCNativeData, XEvent & pEvent )
	{
		if( XEventsQueued( pSCNativeData.display, QueuedAfterReading ) > 0 )
		{
			XNextEvent( pSCNativeData.display, &pEvent );
			return true;
		}

		return false;
	}

	bool _x11EventWait( SysContextNativeData & pSCNativeData, XEvent & pEvent )
	{
		if( XEventsQueued( pSCNativeData.display, QueuedAfterFlush ) > 0 )
		{
			XNextEvent( pSCNativeData.display, &pEvent );
			return true;
		}

		return false;
	}

	bool _x11TranslateAndDispatch( SysEventController & pEventController, XEvent & pXEvent )
	{
		auto & dispatcher = pEventController.getActiveDispatcher();

		SysEvent sysEvent;

		if( ( pNativeEvent.type >= KeyPress ) && ( pNativeEvent.type <= MotionNotify ) )
		{
			auto & inputState = dispatcher.getInputState();
			if( !_x11TranslateInputEvent( inputState, pXEvent, sysEvent ) )
			{
				return false;
			}
		}
		else if( ( pNativeEvent.type >= EnterNotify ) && ( pNativeEvent.type <= MapRequest ) )
		{
			if( !_x11TranslateGenericEvent( dispatcher, pXEvent, sysEvent ) )
			{
				return false;
			}
		}
		else
		{
			if( !_x11TranslateSystemEvent( dispatcher, pXEvent, sysEvent ) )
			{
				return false;
			}
		}

		pEventController.dispatchEvent( sysEvent );

		return true;
	}

	bool _x11TranslateGenericEvent( SysEventDispatcher & pDispatcher, XEvent & pXEvent, SysEvent & pEvent )
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

	bool _x11TranslateInputEvent( SysEventInputState & pInputState, XEvent & pXEvent, SysEvent & pEvent )
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
				auto keycode = _x11GetSystemKeyCode( keysym );
				auto & eventData = pEvent.eInputKeyboardKey;
				eventData.eventCode = E_SYS_EVENT_CODE_INPUT_KEYBOARD_KEY;
				eventData.keyboardState = &( pInputState.keyboardState );
				eventData.keyCode = keycode;
				eventData.keyAction = SysKeyActionType::Press;
				break;
			}
			case KeyRelease:
			{
				auto keysym = XLookupKeysym( &( pXEvent.xkey ), 0 );
				auto keycode = _x11GetSystemKeyCode( keysym );
				auto & eventData = pEvent.eInputKeyboardKey;
				eventData.eventCode = E_SYS_EVENT_CODE_INPUT_KEYBOARD_KEY;
				eventData.keyboardState = &( pInputState.keyboardState );
				eventData.keyCode = keycode;
				eventData.keyAction = SysKeyActionType::Release;

				break;
			}
			case ButtonPress:
			{
				_x11TranslateInputEventMouseButton( pInputState, pXEvent, pEvent, SysMouseButtonActionType::Click );

				break;
			}
			case ButtonRelease:
			{
				_x11TranslateInputEventMouseButton( pInputState, pXEvent, pEvent, SysMouseButtonActionType::Release );

				break;
			}
			case MotionNotify:
			{
				const math::Vec2i32 cursorPos { pXEvent.xmotion.x, pXEvent.xmotion.y };

				if ( pInputState.mouseLastRegPos == cvSysMousePosInvalid )
				{
					pInputState.mouseLastRegPos = cursorPos;
				}

				auto & eInputMouseMove = pEvent.eInputMouseMove;
				eInputMouseMove.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_MOVE;
				eInputMouseMove.cursorPos = cursorPos;

				// Old method, based on the state mask provided by the system. Although *some* modifiers are there,
				// extra mouse buttons (X1 and X2) are not reported. Since there is our own internal state mask kept
				// inside the SysEventInputState object, we just use that. This line is supposed to be kept here just
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

		return pEvent.commonData.eventCode != E_SYS_EVENT_CODE_UNDEFINED;
	}

	bool _x11TranslateInputEventMouseButton( SysEventInputState & pInputState, XEvent & pXEvent, SysEvent & pEvent, SysMouseButtonActionType pButtonAction )
	{
		const math::Vec2i32 cursorPos { pXEvent.xbutton.x, pXEvent.xbutton.y };

		auto SysX11MouseButtonID = _x11GetMouseButtonID( pXEvent.xbutton.button );

		switch( SysX11MouseButtonID )
		{
			case SysX11MouseButtonID::Left:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = SysMouseButtonID::Left;

				if( pButtonAction == SysMouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_LEFT_BIT );
				}
				else if( pButtonAction == SysMouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_LEFT_BIT );
				}

				break;
			}
			case SysX11MouseButtonID::Middle:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = SysMouseButtonID::Middle;

				if( pButtonAction == SysMouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT );
				}
				else if( pButtonAction == SysMouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT );
				}

				break;
			}
			case SysX11MouseButtonID::Right:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = SysMouseButtonID::Right;

				if( pButtonAction == SysMouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT );
				}
				else if( pButtonAction == SysMouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT );
				}

				break;
			}
			case SysX11MouseButtonID::VWheelUp:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = 0;
				eInputMouseScroll.scrollDelta.y = 100;
				break;
			}
			case SysX11MouseButtonID::VWheelDown:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = 0;
				eInputMouseScroll.scrollDelta.y = -100;

				break;
			}
			case SysX11MouseButtonID::HWheelLeft:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = -100;
				eInputMouseScroll.scrollDelta.y = 0;

				break;
			}
			case SysX11MouseButtonID::HWheelRight:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = 100;
				eInputMouseScroll.scrollDelta.y = 0;

				break;
			}
			case SysX11MouseButtonID::Xbtn1:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = SysMouseButtonID::XB1;

				if( pButtonAction == SysMouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_XB1_BIT );
				}
				else if( pButtonAction == SysMouseButtonActionType::Release )
				{
					pInputState.mouseButtonStateMask.unset( SYS_MOUSE_BUTTON_FLAG_XB1_BIT );
				}

				break;
			}
			case SysX11MouseButtonID::Xbtn2:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = pButtonAction;
				eInputMouseButton.buttonID = SysMouseButtonID::XB2;

				if( pButtonAction == SysMouseButtonActionType::Click )
				{
					pInputState.mouseButtonStateMask.set( SYS_MOUSE_BUTTON_FLAG_XB2_BIT );
				}
				else if( pButtonAction == SysMouseButtonActionType::Release )
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

	bool _x11TranslateSystemEvent( SysEventDispatcher & pDispatcher, XEvent & pXEvent, SysEvent & pEvent )
	{
		switch( pXEvent.type )
		{
			case ClientMessage:
			{
				auto * scNativeData = pDispatcher.systemContext->nativeData;
				// Type of wm protocol message is stored in data.l[0].
				long wmpMessageType = pXEvent.xclient.data.l[0];
				if ( wmpMessageType == scNativeData->wmpDeleteWindow )
				{
					auto & eventData = pEvent.eWindowUpdateClose;
					eventData.eventCode = E_SYS_EVENT_CODE_WINDOW_UPDATE_CLOSE;
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


	static const SysKeyCode x11KeycodeMap_20_7A[] =
	{
		/* 0x0020 */ SysKeyCode::Space,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x0030 */ SysKeyCode::Num0,
		/* 0x0031 */ SysKeyCode::Num1,
		/* 0x0032 */ SysKeyCode::Num2,
		/* 0x0033 */ SysKeyCode::Num3,
		/* 0x0034 */ SysKeyCode::Num4,
		/* 0x0035 */ SysKeyCode::Num5,
		/* 0x0036 */ SysKeyCode::Num6,
		/* 0x0037 */ SysKeyCode::Num7,
		/* 0x0038 */ SysKeyCode::Num8,
		/* 0x0039 */ SysKeyCode::Num9,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x0041 */ SysKeyCode::CharA,
		/* 0x0042 */ SysKeyCode::CharB,
		/* 0x0043 */ SysKeyCode::CharC,
		/* 0x0044 */ SysKeyCode::CharD,
		/* 0x0045 */ SysKeyCode::CharE,
		/* 0x0046 */ SysKeyCode::CharF,
		/* 0x0047 */ SysKeyCode::CharG,
		/* 0x0048 */ SysKeyCode::CharH,
		/* 0x0049 */ SysKeyCode::CharI,
		/* 0x004a */ SysKeyCode::CharJ,
		/* 0x004b */ SysKeyCode::CharK,
		/* 0x004c */ SysKeyCode::CharL,
		/* 0x004d */ SysKeyCode::CharM,
		/* 0x004e */ SysKeyCode::CharN,
		/* 0x004f */ SysKeyCode::CharO,
		/* 0x0050 */ SysKeyCode::CharP,
		/* 0x0051 */ SysKeyCode::CharQ,
		/* 0x0052 */ SysKeyCode::CharR,
		/* 0x0053 */ SysKeyCode::CharS,
		/* 0x0054 */ SysKeyCode::CharT,
		/* 0x0055 */ SysKeyCode::CharU,
		/* 0x0056 */ SysKeyCode::CharV,
		/* 0x0057 */ SysKeyCode::CharW,
		/* 0x0058 */ SysKeyCode::CharX,
		/* 0x0059 */ SysKeyCode::CharY,
		/* 0x005a */ SysKeyCode::CharZ,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x0061 */ SysKeyCode::CharA,
		/* 0x0062 */ SysKeyCode::CharB,
		/* 0x0063 */ SysKeyCode::CharC,
		/* 0x0064 */ SysKeyCode::CharD,
		/* 0x0065 */ SysKeyCode::CharE,
		/* 0x0066 */ SysKeyCode::CharF,
		/* 0x0067 */ SysKeyCode::CharG,
		/* 0x0068 */ SysKeyCode::CharH,
		/* 0x0069 */ SysKeyCode::CharI,
		/* 0x006a */ SysKeyCode::CharJ,
		/* 0x006b */ SysKeyCode::CharK,
		/* 0x006c */ SysKeyCode::CharL,
		/* 0x006d */ SysKeyCode::CharM,
		/* 0x006e */ SysKeyCode::CharN,
		/* 0x006f */ SysKeyCode::CharO,
		/* 0x0070 */ SysKeyCode::CharP,
		/* 0x0071 */ SysKeyCode::CharQ,
		/* 0x0072 */ SysKeyCode::CharR,
		/* 0x0073 */ SysKeyCode::CharS,
		/* 0x0074 */ SysKeyCode::CharT,
		/* 0x0075 */ SysKeyCode::CharU,
		/* 0x0076 */ SysKeyCode::CharV,
		/* 0x0077 */ SysKeyCode::CharW,
		/* 0x0078 */ SysKeyCode::CharX,
		/* 0x0079 */ SysKeyCode::CharY,
		/* 0x007a */ SysKeyCode::CharZ,
	};

	static const SysKeyCode x11KeycodeMap_50_57[] =
	{
		/* 0xff50 */ SysKeyCode::Home,
		/* 0xff51 */ SysKeyCode::ArrowLeft,
		/* 0xff52 */ SysKeyCode::ArrowUp,
		/* 0xff53 */ SysKeyCode::ArrowRight,
		/* 0xff54 */ SysKeyCode::ArrowDown,
		/* 0xff55 */ SysKeyCode::PageUp,
		/* 0xff56 */ SysKeyCode::PageDown,
		/* 0xff57 */ SysKeyCode::End
	};

	static const SysKeyCode x11KeycodeMap_AA_B9[] =
	{
		/* 0xffaa */ SysKeyCode::NumpadMul,
		/* 0xffab */ SysKeyCode::NumpadAdd,
		/* 0xffac */ SysKeyCode::Unknown,
		/* 0xffad */ SysKeyCode::NumpadSub,
		/* 0xffae */ SysKeyCode::NumpadDot,
		/* 0xffaf */ SysKeyCode::NumpadDiv,
		/* 0xffb0 */ SysKeyCode::Numpad0,
		/* 0xffb1 */ SysKeyCode::Numpad1,
		/* 0xffb2 */ SysKeyCode::Numpad2,
		/* 0xffb3 */ SysKeyCode::Numpad3,
		/* 0xffb4 */ SysKeyCode::Numpad4,
		/* 0xffb5 */ SysKeyCode::Numpad5,
		/* 0xffb6 */ SysKeyCode::Numpad6,
		/* 0xffb7 */ SysKeyCode::Numpad7,
		/* 0xffb8 */ SysKeyCode::Numpad8,
		/* 0xffb9 */ SysKeyCode::Numpad9,
	};

	static const SysKeyCode x11KeycodeMap_BE_C9[] =
	{
		/* 0xffbe */ SysKeyCode::F1,
		/* 0xffbf */ SysKeyCode::F2,
		/* 0xffc0 */ SysKeyCode::F3,
		/* 0xffc1 */ SysKeyCode::F4,
		/* 0xffc2 */ SysKeyCode::F5,
		/* 0xffc3 */ SysKeyCode::F6,
		/* 0xffc4 */ SysKeyCode::F7,
		/* 0xffc5 */ SysKeyCode::F8,
		/* 0xffc6 */ SysKeyCode::F9,
		/* 0xffc7 */ SysKeyCode::F10,
		/* 0xffc8 */ SysKeyCode::F11,
		/* 0xffc9 */ SysKeyCode::F12
	};

	static const SysKeyCode x11KeycodeMap_E1_EA[] =
	{
		/* 0xffe1 */ SysKeyCode::ShiftLeft,
		/* 0xffe2 */ SysKeyCode::ShiftRight,
		/* 0xffe3 */ SysKeyCode::CtrlLeft,
		/* 0xffe4 */ SysKeyCode::CtrlRight,
		/* 0xffe5 */ SysKeyCode::CapsLock,
		/* 0xffe6 */ SysKeyCode::Unknown,
		/* 0xffe7 */ SysKeyCode::Unknown,
		/* 0xffe8 */ SysKeyCode::Unknown,
		/* 0xffe9 */ SysKeyCode::AltLeft,
		/* 0xffea */ SysKeyCode::AltRight
	};


	SysKeyCode _x11GetSystemKeyCode( KeySym pXkeySym )
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
			return SysKeyCode::Tab;
		}
		else if ( pXkeySym == XK_Return )
		{
			return SysKeyCode::Enter;
		}
		else if ( pXkeySym == XK_Escape )
		{
			return SysKeyCode::Escape;
		}
		else if ( pXkeySym == XK_BackSpace )
		{
			return SysKeyCode::Backspace;
		}
		else if ( pXkeySym == XK_Insert )
		{
			return SysKeyCode::Insert;
		}
		else if ( pXkeySym == XK_Delete )
		{
			return SysKeyCode::Delete;
		}

		return SysKeyCode::Unknown;
	}

	static constexpr SysX11MouseButtonID x11MouseButtonIDMap[] =
	{
		SysX11MouseButtonID::Left,
		SysX11MouseButtonID::Middle,
		SysX11MouseButtonID::Right,
		SysX11MouseButtonID::VWheelUp,
		SysX11MouseButtonID::VWheelDown,
		SysX11MouseButtonID::HWheelLeft,
		SysX11MouseButtonID::HWheelRight,
		SysX11MouseButtonID::Xbtn1,
		SysX11MouseButtonID::Xbtn2,
		SysX11MouseButtonID::Unknown
	};
	static constexpr auto x11MouseButtonMaxIndex = staticArraySize( x11MouseButtonIDMap ) - 1;

	SysX11MouseButtonID _x11GetMouseButtonID( uint32 pButton )
	{
		auto mouseButtonIndex = static_cast<size_t>( pButton - 1 );
		mouseButtonIndex = getMinOf( mouseButtonIndex, x11MouseButtonMaxIndex );
		return x11MouseButtonIDMap[mouseButtonIndex];
	}

	Bitmask<SysMouseButtonFlagBits> _x11GetMouseButtonStateMask( uint32 pState )
	{
		Bitmask<uint32> inputStateMask = pState;
		Bitmask<SysMouseButtonFlagBits> buttonStateMask;
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
