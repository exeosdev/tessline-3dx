
#include <ts3/system/eventCore.h>
#include <ts3/math/vectorOps.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3
{

	bool _win32TranslateAndDispatch( SysEventController & pEventController, const MSG & pMSG );
	bool _win32TranslateAppOrWindowEvent( SysEventDispatcher & pEventDispatcher, const MSG & pMSG, SysEvent & pEvent );
	bool _win32TranslateInputEvent( SysEventInputState & pInputState, const MSG & pMSG, SysEvent & pEvent );
	bool _win32TranslateInputEventKeyboard( SysEventInputState & pInputState, const MSG & pMSG, SysEvent & pEvent );
	bool _win32TranslateInputEventMouse( SysEventInputState & pInputState, const MSG & pMSG, SysEvent & pEvent );
	bool _win32TranslateInputEventTouch( SysEventInputState & pInputState, const MSG & pMSG, SysEvent & pEvent );
	SysKeyCode _win32GetSystemKeyCode( WPARAM pWparam );
	Bitmask<SysMouseButtonFlagBits> _win32GetMouseButtonStateMask( WPARAM pWparam );
	LRESULT __stdcall _win32EventSourceIdleEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );
	LRESULT __stdcall _win32EventSourceProxyEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );


	void SysEventController::_sysInitialize()
	{}

	void SysEventController::_sysRelease()
	{}

	void SysEventController::_sysAddEventSource( SysEventSource & pEventSource )
	{
		auto eventControllerAddress = reinterpret_cast<LONG_PTR>( this );
		::SetWindowLongPtrA( pEventSource.mEvtSrcNativeData.hwnd, GWLP_USERDATA, eventControllerAddress );

		auto wndEventProcAddress = reinterpret_cast<LONG_PTR>( _win32EventSourceProxyEventProc );
		::SetWindowLongPtrA( pEventSource.mEvtSrcNativeData.hwnd, GWLP_WNDPROC, wndEventProcAddress );

		auto updateFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
		// Trigger the update of control's internals.
		::SetWindowPos( pEventSource.mEvtSrcNativeData.hwnd, nullptr, 0, 0, 0, 0, updateFlags );
	}

	void SysEventController::_sysRemoveEventSource( SysEventSource & pEventSource )
	{
		auto wndEventProcAddress = reinterpret_cast<LONG_PTR>( _win32EventSourceIdleEventProc );
		::SetWindowLongPtrA( pEventSource.mEvtSrcNativeData.hwnd, GWLP_WNDPROC, wndEventProcAddress );

		auto updateFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
		// Trigger the update of control's internals.
		::SetWindowPos( pEventSource.mEvtSrcNativeData.hwnd, nullptr, 0, 0, 0, 0, updateFlags );
	}

	void SysEventController::_sysDispatchNextEvent()
	{
		MSG pMSG;
		if( ::PeekMessageA( &pMSG, nullptr, 0, 0, PM_REMOVE ) != FALSE )
		{
			::TranslateMessage( &pMSG );
			::DispatchMessageA( &pMSG );
		}
	}

	void SysEventController::_sysDispatchNextEventWait()
	{
		MSG pMSG;
		if( ::GetMessageA( &pMSG, nullptr, 0, 0 ) != FALSE )
		{
			::TranslateMessage( &pMSG );
			::DispatchMessageA( &pMSG );
		}
	}

	void SysEventController::_sysDispatchQueuedEvents()
	{
		MSG pMSG;
		while( ::PeekMessageA( &pMSG, nullptr, 0, 0, PM_REMOVE ) != FALSE )
		{
			::TranslateMessage( &pMSG );
			::DispatchMessageA( &pMSG );
		}
	}

	void SysEventController::_sysDispatchQueuedEventsWait()
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



	bool _win32TranslateAndDispatch( SysEventController & pEventController, const MSG & pMSG )
	{
		auto & dispatcher = pEventController.getActiveDispatcher();

		SysEvent sysEvent;

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

	bool _win32TranslateAppOrWindowEvent( SysEventDispatcher & pEventDispatcher, const MSG & pMSG, SysEvent & pEvent )
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
				eWindowUpdateClose.eventCode = E_SYS_EVENT_CODE_WINDOW_UPDATE_CLOSE;
				break;
			}
			case WM_QUIT:
			{
				auto & eAppActivityQuit = pEvent.eAppActivityQuit;
				eAppActivityQuit.eventCode = E_SYS_EVENT_CODE_APP_ACTIVITY_QUIT;
				break;
			}
			default:
			{
				return false;
			}
		}

		return true;
	}

	bool _win32TranslateInputEvent( SysEventInputState & pInputState, const MSG & pMSG, SysEvent & pEvent )
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

	bool _win32TranslateInputEventKeyboard( SysEventInputState & pInputState, const MSG & pMSG, SysEvent & pEvent )
	{
		switch( pMSG.message )
		{
			case WM_KEYDOWN:
			{
				auto & eInputKeyboardKey = pEvent.eInputKeyboardKey;
				eInputKeyboardKey.eventCode = E_SYS_EVENT_CODE_INPUT_KEYBOARD_KEY;
				eInputKeyboardKey.keyboardState = &( pInputState.keyboardState );
				eInputKeyboardKey.keyAction = SysKeyActionType::Press;
				eInputKeyboardKey.keyCode = _win32GetSystemKeyCode( pMSG.wParam );
				break;
			}
			case WM_KEYUP:
			{
				auto & eInputKeyboardKey = pEvent.eInputKeyboardKey;
				eInputKeyboardKey.eventCode = E_SYS_EVENT_CODE_INPUT_KEYBOARD_KEY;
				eInputKeyboardKey.keyboardState = &( pInputState.keyboardState );
				eInputKeyboardKey.keyAction = SysKeyActionType::Release;
				eInputKeyboardKey.keyCode = _win32GetSystemKeyCode( pMSG.wParam );
				break;
			}
			default:
			{
				return false;
			}
		}

		return true;
	}

	bool _win32TranslateInputEventMouse( SysEventInputState & pInputState, const MSG & pMSG, SysEvent & pEvent )
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

				if ( pInputState.mouseLastRegPos == cvSysMousePosInvalid )
				{
					pInputState.mouseLastRegPos = cursorPos;
				}

				eInputMouseMove.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_MOVE;
				eInputMouseMove.cursorPos = cursorPos;
				eInputMouseMove.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				eInputMouseMove.movementDelta = cursorPos - pInputState.mouseLastRegPos;
				pInputState.mouseLastRegPos = cursorPos;
				break;
			}
			case WM_LBUTTONDOWN:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = SysMouseButtonActionType::Click;
				eInputMouseButton.buttonID = SysMouseButtonID::Left;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_LBUTTONUP:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = SysMouseButtonActionType::Release;
				eInputMouseButton.buttonID = SysMouseButtonID::Left;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_LBUTTONDBLCLK:
			{
				pEvent.commonData.eventCode = E_SYS_EVENT_CODE_UNDEFINED;
				break;
			}
			case WM_RBUTTONDOWN:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = SysMouseButtonActionType::Click;
				eInputMouseButton.buttonID = SysMouseButtonID::Right;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_RBUTTONUP:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = SysMouseButtonActionType::Release;
				eInputMouseButton.buttonID = SysMouseButtonID::Right;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_RBUTTONDBLCLK:
			{
				pEvent.commonData.eventCode = E_SYS_EVENT_CODE_UNDEFINED;
				break;
			}
			case WM_MBUTTONDOWN:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = SysMouseButtonActionType::Click;
				eInputMouseButton.buttonID = SysMouseButtonID::Middle;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_MBUTTONUP:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = SysMouseButtonActionType::Release;
				eInputMouseButton.buttonID = SysMouseButtonID::Middle;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_MBUTTONDBLCLK:
			{
				pEvent.commonData.eventCode = E_SYS_EVENT_CODE_UNDEFINED;
				break;
			}
			case WM_MOUSEWHEEL:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				pEvent.code = E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = 0;
				eInputMouseScroll.scrollDelta.y = static_cast<int32>( GET_WHEEL_DELTA_WPARAM( pMSG.wParam ) );
				break;
			}
			case WM_XBUTTONDOWN:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = SysMouseButtonActionType::Click;
				eInputMouseButton.buttonID = ( GET_XBUTTON_WPARAM( pMSG.wParam ) == XBUTTON1 ) ? SysMouseButtonID::XB1 : SysMouseButtonID::XB2;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_XBUTTONUP:
			{
				auto & eInputMouseButton = pEvent.eInputMouseButton;
				eInputMouseButton.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON;
				eInputMouseButton.cursorPos = cursorPos;
				eInputMouseButton.buttonAction = SysMouseButtonActionType::Release;
				eInputMouseButton.buttonID = ( GET_XBUTTON_WPARAM( pMSG.wParam ) == XBUTTON1 ) ? SysMouseButtonID::XB1 : SysMouseButtonID::XB2;
				eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
				break;
			}
			case WM_XBUTTONDBLCLK:
			{
				pEvent.commonData.eventCode = E_SYS_EVENT_CODE_UNDEFINED;
				break;
			}
			case WM_MOUSEHWHEEL:
			{
				auto & eInputMouseScroll = pEvent.eInputMouseScroll;
				eInputMouseScroll.eventCode = E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL;
				eInputMouseScroll.scrollDelta.x = static_cast<int32>( GET_WHEEL_DELTA_WPARAM( pMSG.wParam ) );
				eInputMouseScroll.scrollDelta.y = 0;
				break;
			}
		}

		return pEvent.commonData.eventCode != E_SYS_EVENT_CODE_UNDEFINED;
	}

#if defined( WM_TOUCH )
	bool _win32TranslateInputEventTouch( SysEventInputState & pInputState, const MSG & pMSG, SysEvent & pEvent )
	{
		return false;
	}
#endif

	static const SysKeyCode asciiKeyCodeMap_08_7B[] =
	{
		/* 0x0008 */ SysKeyCode::Backspace,
		/* 0x00xx */ SysKeyCode::Tab,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Enter,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::ShiftLeft,
		/* 0x00xx */ SysKeyCode::CtrlLeft,
		/* 0x00xx */ SysKeyCode::AltLeft,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x0014 */ SysKeyCode::CapsLock,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x001B */ SysKeyCode::Escape,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x0020 */ SysKeyCode::Space,
		/* 0x0021 */ SysKeyCode::PageUp,
		/* 0x0022 */ SysKeyCode::PageDown,
		/* 0x0023 */ SysKeyCode::End,
		/* 0x0024 */ SysKeyCode::Home,
		/* 0x0025 */ SysKeyCode::ArrowLeft,
		/* 0x0026 */ SysKeyCode::ArrowUp,
		/* 0x0027 */ SysKeyCode::ArrowRight,
		/* 0x0028 */ SysKeyCode::ArrowDown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x002D */ SysKeyCode::Insert,
		/* 0x002E */ SysKeyCode::Delete,
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
		/* 0x004A */ SysKeyCode::CharJ,
		/* 0x004B */ SysKeyCode::CharK,
		/* 0x004C */ SysKeyCode::CharL,
		/* 0x004D */ SysKeyCode::CharM,
		/* 0x004E */ SysKeyCode::CharN,
		/* 0x004F */ SysKeyCode::CharO,
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
		/* 0x005A */ SysKeyCode::CharZ,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x00xx */ SysKeyCode::Unknown,
		/* 0x0060 */ SysKeyCode::Numpad0,
		/* 0x0061 */ SysKeyCode::Numpad1,
		/* 0x0062 */ SysKeyCode::Numpad2,
		/* 0x0063 */ SysKeyCode::Numpad3,
		/* 0x0064 */ SysKeyCode::Numpad4,
		/* 0x0065 */ SysKeyCode::Numpad5,
		/* 0x0066 */ SysKeyCode::Numpad6,
		/* 0x0067 */ SysKeyCode::Numpad7,
		/* 0x0068 */ SysKeyCode::Numpad8,
		/* 0x0069 */ SysKeyCode::Numpad9,
		/* 0x006A */ SysKeyCode::NumpadMul,
		/* 0x006B */ SysKeyCode::NumpadAdd,
		/* 0x006C */ SysKeyCode::Unknown,
		/* 0x006D */ SysKeyCode::NumpadSub,
		/* 0x006E */ SysKeyCode::NumpadDot,
		/* 0x006F */ SysKeyCode::NumpadDiv,
		/* 0x0070 */ SysKeyCode::F1,
		/* 0x0071 */ SysKeyCode::F2,
		/* 0x0072 */ SysKeyCode::F3,
		/* 0x0073 */ SysKeyCode::F4,
		/* 0x0074 */ SysKeyCode::F5,
		/* 0x0075 */ SysKeyCode::F6,
		/* 0x0076 */ SysKeyCode::F7,
		/* 0x0077 */ SysKeyCode::F8,
		/* 0x0078 */ SysKeyCode::F9,
		/* 0x0079 */ SysKeyCode::F10,
		/* 0x007A */ SysKeyCode::F11,
		/* 0x007B */ SysKeyCode::F12
	};


	static const SysKeyCode asciiKeyCodeMap_A0_A5[] =
	{
		/* 0x00A0 */ SysKeyCode::ShiftLeft,
		/* 0x00A1 */ SysKeyCode::ShiftRight,
		/* 0x00A2 */ SysKeyCode::CtrlLeft,
		/* 0x00A3 */ SysKeyCode::CtrlRight,
		/* 0x00A4 */ SysKeyCode::AltLeft,
		/* 0x00A5 */ SysKeyCode::AltRight,
	};

	SysKeyCode _win32GetSystemKeyCode( WPARAM pWparam )
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

		return SysKeyCode::Unknown;
	}

	Bitmask<SysMouseButtonFlagBits> _win32GetMouseButtonStateMask( WPARAM pWparam )
	{
		return static_cast<SysMouseButtonFlagBits>( static_cast<uint32>( pWparam ) & SYS_MOUSE_BUTTON_FLAG_ALL_BIT );
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

			auto * eventController = reinterpret_cast<SysEventController *>( windowUserData );
			_win32TranslateAndDispatch( *eventController, systemEvent );
		}

		return ::DefWindowProcA( pHWND, pMessage, pWparam, pLparam );
	}

}
#endif
