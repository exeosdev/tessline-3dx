
#include <ts3/system/eventCore.h>
#include <ts3/math/vectorOps.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

	bool _win32TranslateAndDispatch( EventController & pEventController, const MSG & pMSG );
	bool _win32TranslateAppOrWindowEvent( EventDispatcher & pEventDispatcher, const MSG & pMSG, Event & pEvent );
	bool _win32TranslateInputEvent( EventInputState & pInputState, const MSG & pMSG, Event & pEvent );
	bool _win32TranslateInputEventKeyboard( EventInputState & pInputState, const MSG & pMSG, Event & pEvent );
	bool _win32TranslateInputEventMouse( EventInputState & pInputState, const MSG & pMSG, Event & pEvent );
	bool _win32TranslateInputEventTouch( EventInputState & pInputState, const MSG & pMSG, Event & pEvent );
	KeyCode _win32GettemKeyCode( WPARAM pWparam );
	Bitmask<MouseButtonFlagBits> _win32GetMouseButtonStateMask( WPARAM pWparam );
	LRESULT __stdcall _win32EventSourceIdleEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );
	LRESULT __stdcall _win32EventSourceProxyEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );


	void EventController::_sysInitialize()
	{}

	void EventController::_sysRelease()
	{}

	void EventController::_sysAddEventSource( EventSource & pEventSource )
	{
		auto eventControllerAddress = reinterpret_cast<LONG_PTR>( this );
		::SetWindowLongPtrA( pEventSource.mEvtSrcNativeData.hwnd, GWLP_USERDATA, eventControllerAddress );

		auto wndEventProcAddress = reinterpret_cast<LONG_PTR>( _win32EventSourceProxyEventProc );
		::SetWindowLongPtrA( pEventSource.mEvtSrcNativeData.hwnd, GWLP_WNDPROC, wndEventProcAddress );

		auto updateFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
		// Trigger the update of control's internals.
		::SetWindowPos( pEventSource.mEvtSrcNativeData.hwnd, nullptr, 0, 0, 0, 0, updateFlags );
	}

	void EventController::_sysRemoveEventSource( EventSource & pEventSource )
	{
		auto wndEventProcAddress = reinterpret_cast<LONG_PTR>( _win32EventSourceIdleEventProc );
		::SetWindowLongPtrA( pEventSource.mEvtSrcNativeData.hwnd, GWLP_WNDPROC, wndEventProcAddress );

		auto updateFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
		// Trigger the update of control's internals.
		::SetWindowPos( pEventSource.mEvtSrcNativeData.hwnd, nullptr, 0, 0, 0, 0, updateFlags );
	}

	void EventController::_sysDispatchNextEvent()
	{
		MSG pMSG;
		if( ::PeekMessageA( &pMSG, nullptr, 0, 0, PM_REMOVE ) != FALSE )
		{
			::TranslateMessage( &pMSG );
			::DispatchMessageA( &pMSG );
		}
	}

	void EventController::_sysDispatchNextEventWait()
	{
		MSG pMSG;
		if( ::GetMessageA( &pMSG, nullptr, 0, 0 ) != FALSE )
		{
			::TranslateMessage( &pMSG );
			::DispatchMessageA( &pMSG );
		}
	}

	void EventController::_sysDispatchQueuedEvents()
	{
		MSG pMSG;
		while( ::PeekMessageA( &pMSG, nullptr, 0, 0, PM_REMOVE ) != FALSE )
		{
			::TranslateMessage( &pMSG );
			::DispatchMessageA( &pMSG );
		}
	}

	void EventController::_sysDispatchQueuedEventsWait()
	{
		uint32 fetchedEventsNum = 0;

		MSG pMSG;
		while( ::PeekMessageA( &pMSG, nullptr, 0, 0, PM_REMOVE ) != FALSE )
		{
			::TranslateMessage( &pMSG );
			::DispatchMessageA( &pMSG );
			++fetchedEventsNum;
		}

		if( ( fetchedEventsNum == 0 ) && ::GetMessageA( &pMSG, nullptr, 0, 0 ) != FALSE )
		{
			::TranslateMessage( &pMSG );
			::DispatchMessageA( &pMSG );
		}
	}



	bool _win32TranslateAndDispatch( EventController & pEventController, const MSG & pMSG )
	{
		auto & dispatcher = pEventController.getActiveDispatcher();

		Event sysEvent;

		if( pMSG.message >= WM_KEYFIRST && pMSG.message <= WM_TOUCH )
		{
			auto & inputState = dispatcher.getInputState();
			if( !_win32TranslateInputEvent( inputState, pMSG, sysEvent ) )
			{
				return false;
			}
		}
		else
		{
			if( !_win32TranslateAppOrWindowEvent( dispatcher, pMSG, sysEvent ) )
			{
				return false;
			}
		}

		pEventController.dispatchEvent( sysEvent );

		return true;
	}

	bool _win32TranslateAppOrWindowEvent( EventDispatcher & pEventDispatcher, const MSG & pMSG, Event & pEvent )
	{
		switch ( pMSG.message )
		{
			case WM_CLOSE:
			{
				::DestroyWindow( pMSG.hwnd );
				break;
			}
			case WM_DESTROY:
			{
				auto & eWindowUpdateClose = pEvent.eWindowUpdateClose;
				eWindowUpdateClose.eventCode = E_EVENT_CODE_WINDOW_UPDATE_CLOSE;
				break;
			}
			case WM_QUIT:
			{
				auto & eAppActivityQuit = pEvent.eAppActivityQuit;
				eAppActivityQuit.eventCode = E_EVENT_CODE_APP_ACTIVITY_QUIT;
				break;
			}
			default:
			{
				return false;
			}
		}

		return true;
	}

	bool _win32TranslateInputEvent( EventInputState & pInputState, const MSG & pMSG, Event & pEvent )
	{
		if( ( pMSG.message >= WM_KEYFIRST ) && ( pMSG.message <= WM_KEYLAST ) )
		{
			return _win32TranslateInputEventKeyboard( pInputState, pMSG, pEvent );
		}
		else if( ( pMSG.message >= WM_MOUSEFIRST ) && ( pMSG.message <= WM_MOUSELAST ) )
		{
			return _win32TranslateInputEventMouse( pInputState, pMSG, pEvent );
		}
	#if defined( WM_TOUCH )
		else if( pMSG.message == WM_TOUCH )
		{
			return _win32TranslateInputEventTouch( pInputState, pMSG, pEvent );
		}
	#endif
		else
		{
			return false;
		}
	}

	bool _win32TranslateInputEventKeyboard( EventInputState & pInputState, const MSG & pMSG, Event & pEvent )
	{
		switch( pMSG.message )
		{
			case WM_KEYDOWN:
			{
				auto & eInputKeyboardKey = pEvent.eInputKeyboardKey;
				eInputKeyboardKey.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
				eInputKeyboardKey.keyboardState = &( pInputState.keyboardState );
				eInputKeyboardKey.keyAction = KeyActionType::Press;
				eInputKeyboardKey.keyCode = _win32GettemKeyCode( pMSG.wParam );
				break;
			}
			case WM_KEYUP:
			{
				auto & eInputKeyboardKey = pEvent.eInputKeyboardKey;
				eInputKeyboardKey.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
				eInputKeyboardKey.keyboardState = &( pInputState.keyboardState );
				eInputKeyboardKey.keyAction = KeyActionType::Release;
				eInputKeyboardKey.keyCode = _win32GettemKeyCode( pMSG.wParam );
				break;
			}
			default:
			{
				return false;
			}
		}

		return true;
	}

	bool _win32TranslateInputEventMouse( EventInputState & pInputState, const MSG & pMSG, Event & pEvent )
	{
		math::Vec2i32 cursorPos {
			GET_X_LPARAM( pMSG.lParam ),
			GET_Y_LPARAM( pMSG.lParam )
		};

		switch( pMSG.message )
		{
			case WM_MOUSEMOVE:
			{
				auto & eInputMouseMove = pEvent.eInputMouseMove;

				if ( pInputState.mouseLastRegPos == cvMousePosInvalid )
				{
					pInputState.mouseLastRegPos = cursorPos;
				}

				eInputMouseMove.eventCode = E_EVENT_CODE_INPUT_MOUSE_MOVE;
				eInputMouseMove.cursorPos = cursorPos;
				eInputMouseMove.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				eInputMouseMove.movementDelta = cursorPos - pInputState.mouseLastRegPos;
				pInputState.mouseLastRegPos = cursorPos;
				break;
			}
			case WM_LBUTTONDOWN:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = MouseButtonActionType::Click;
				eInputMouseButton.buttonID = MouseButtonID::Left;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_LBUTTONUP:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = MouseButtonActionType::Release;
				eInputMouseButton.buttonID = MouseButtonID::Left;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_LBUTTONDBLCLK:
			{
				pEvent.commonData.eventCode = E_EVENT_CODE_UNDEFINED;
				break;
			}
			case WM_RBUTTONDOWN:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = MouseButtonActionType::Click;
				eInputMouseButton.buttonID = MouseButtonID::Right;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_RBUTTONUP:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = MouseButtonActionType::Release;
				eInputMouseButton.buttonID = MouseButtonID::Right;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_RBUTTONDBLCLK:
			{
				pEvent.commonData.eventCode = E_EVENT_CODE_UNDEFINED;
				break;
			}
			case WM_MBUTTONDOWN:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = MouseButtonActionType::Click;
				eInputMouseButton.buttonID = MouseButtonID::Middle;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_MBUTTONUP:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = MouseButtonActionType::Release;
				eInputMouseButton.buttonID = MouseButtonID::Middle;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_MBUTTONDBLCLK:
			{
				pEvent.commonData.eventCode = E_EVENT_CODE_UNDEFINED;
				break;
			}
			case WM_MOUSEWHEEL:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				pEvent.code = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = 0;
				eInputMouseScroll.scrollDelta.y = static_cast<int32>( GET_WHEEL_DELTA_WPARAM( pMSG.wParam ) );
				break;
			}
			case WM_XBUTTONDOWN:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = MouseButtonActionType::Click;
				eInputMouseButton.buttonID = ( GET_XBUTTON_WPARAM( pMSG.wParam ) == XBUTTON1 ) ? MouseButtonID::XB1 : MouseButtonID::XB2;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_XBUTTONUP:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = MouseButtonActionType::Release;
				eInputMouseButton.buttonID = ( GET_XBUTTON_WPARAM( pMSG.wParam ) == XBUTTON1 ) ? MouseButtonID::XB1 : MouseButtonID::XB2;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_XBUTTONDBLCLK:
			{
				pEvent.commonData.eventCode = E_EVENT_CODE_UNDEFINED;
				break;
			}
			case WM_MOUSEHWHEEL:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = static_cast<int32>( GET_WHEEL_DELTA_WPARAM( pMSG.wParam ) );
				eInputMouseScroll.scrollDelta.y = 0;
				break;
			}
		}

		return pEvent.commonData.eventCode != E_EVENT_CODE_UNDEFINED;
	}

#if defined( WM_TOUCH )
	bool _win32TranslateInputEventTouch( EventInputState & pInputState, const MSG & pMSG, Event & pEvent )
	{
		return false;
	}
#endif

	static const KeyCode asciiKeyCodeMap_08_7B[] =
	{
		/* 0x0008 */ KeyCode::Backspace,
		/* 0x00xx */ KeyCode::Tab,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Enter,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::ShiftLeft,
		/* 0x00xx */ KeyCode::CtrlLeft,
		/* 0x00xx */ KeyCode::AltLeft,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x0014 */ KeyCode::CapsLock,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x001B */ KeyCode::Escape,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x0020 */ KeyCode::Space,
		/* 0x0021 */ KeyCode::PageUp,
		/* 0x0022 */ KeyCode::PageDown,
		/* 0x0023 */ KeyCode::End,
		/* 0x0024 */ KeyCode::Home,
		/* 0x0025 */ KeyCode::ArrowLeft,
		/* 0x0026 */ KeyCode::ArrowUp,
		/* 0x0027 */ KeyCode::ArrowRight,
		/* 0x0028 */ KeyCode::ArrowDown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x002D */ KeyCode::Insert,
		/* 0x002E */ KeyCode::Delete,
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
		/* 0x004A */ KeyCode::CharJ,
		/* 0x004B */ KeyCode::CharK,
		/* 0x004C */ KeyCode::CharL,
		/* 0x004D */ KeyCode::CharM,
		/* 0x004E */ KeyCode::CharN,
		/* 0x004F */ KeyCode::CharO,
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
		/* 0x005A */ KeyCode::CharZ,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x00xx */ KeyCode::Unknown,
		/* 0x0060 */ KeyCode::Numpad0,
		/* 0x0061 */ KeyCode::Numpad1,
		/* 0x0062 */ KeyCode::Numpad2,
		/* 0x0063 */ KeyCode::Numpad3,
		/* 0x0064 */ KeyCode::Numpad4,
		/* 0x0065 */ KeyCode::Numpad5,
		/* 0x0066 */ KeyCode::Numpad6,
		/* 0x0067 */ KeyCode::Numpad7,
		/* 0x0068 */ KeyCode::Numpad8,
		/* 0x0069 */ KeyCode::Numpad9,
		/* 0x006A */ KeyCode::NumpadMul,
		/* 0x006B */ KeyCode::NumpadAdd,
		/* 0x006C */ KeyCode::Unknown,
		/* 0x006D */ KeyCode::NumpadSub,
		/* 0x006E */ KeyCode::NumpadDot,
		/* 0x006F */ KeyCode::NumpadDiv,
		/* 0x0070 */ KeyCode::F1,
		/* 0x0071 */ KeyCode::F2,
		/* 0x0072 */ KeyCode::F3,
		/* 0x0073 */ KeyCode::F4,
		/* 0x0074 */ KeyCode::F5,
		/* 0x0075 */ KeyCode::F6,
		/* 0x0076 */ KeyCode::F7,
		/* 0x0077 */ KeyCode::F8,
		/* 0x0078 */ KeyCode::F9,
		/* 0x0079 */ KeyCode::F10,
		/* 0x007A */ KeyCode::F11,
		/* 0x007B */ KeyCode::F12
	};


	static const KeyCode asciiKeyCodeMap_A0_A5[] =
	{
		/* 0x00A0 */ KeyCode::ShiftLeft,
		/* 0x00A1 */ KeyCode::ShiftRight,
		/* 0x00A2 */ KeyCode::CtrlLeft,
		/* 0x00A3 */ KeyCode::CtrlRight,
		/* 0x00A4 */ KeyCode::AltLeft,
		/* 0x00A5 */ KeyCode::AltRight,
	};

	KeyCode _win32GettemKeyCode( WPARAM pWparam )
	{
		if ( ( pWparam >= 0x0008 ) && ( pWparam <= 0x007B ) )
		{
			auto baseIndex = pWparam - 0x0008;
			return asciiKeyCodeMap_08_7B[baseIndex];
		}

		if ( ( pWparam >= 0x00A0 ) && ( pWparam <= 0x00A5 ) )
		{
			auto baseIndex = pWparam - 0x00A0;
			return asciiKeyCodeMap_A0_A5[baseIndex];
		}

		return KeyCode::Unknown;
	}

	Bitmask<MouseButtonFlagBits> _win32GetMouseButtonStateMask( WPARAM pWparam )
	{
		return static_cast<MouseButtonFlagBits>( static_cast<uint32>( pWparam ) & SYS_MOUSE_BUTTON_FLAG_ALL_BIT );
	}

	LRESULT __stdcall _win32EventSourceIdleEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam )
	{
		return ::DefWindowProcA( pHWND, pMessage, pWparam, pLparam );
	}

	LRESULT __stdcall _win32EventSourceProxyEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam )
	{
		LONG_PTR windowUserData = ::GetWindowLongPtrA( pHWND, GWLP_USERDATA );
		if ( windowUserData != 0 )
		{
			MSG systemEvent;
			systemEvent.hwnd = pHWND;
			systemEvent.message = pMessage;
			systemEvent.wParam = pWparam;
			systemEvent.lParam = pLparam;

			auto * eventController = reinterpret_cast<EventController *>( windowUserData );
			_win32TranslateAndDispatch( *eventController, systemEvent );
		}

		return ::DefWindowProcA( pHWND, pMessage, pWparam, pLparam );
	}

} // namespace ts3::system
#endif
