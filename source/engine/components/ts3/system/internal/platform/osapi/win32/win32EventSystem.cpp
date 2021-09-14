
#include <ts3/system/eventSystemNative.h>
#include <ts3/system/windowNative.h>
#include <ts3/math/vectorOps.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{


	bool EventController::_nativeDispatchNext()
	{
	    MSG pMSG;
	    if( ::PeekMessageA( &pMSG, nullptr, 0, 0, PM_REMOVE ) != FALSE )
	    {
	        ::TranslateMessage( &pMSG );
	        ::DispatchMessageA( &pMSG );

	        return true;
	    }

	    return false;
	}

	bool EventController::_nativeDispatchNextWait()
	{
	    MSG pMSG;
	    if( ::GetMessageA( &pMSG, nullptr, 0, 0 ) != FALSE )
	    {
	        ::TranslateMessage( &pMSG );
	        ::DispatchMessageA( &pMSG );

	        return true;
	    }

	    return false;
	}

	void EventController::_nativeOnActiveDispatcherChange( EventDispatcher * pDispatcher )
	{}


	bool _win32TranslateAppOrWindowEvent( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );
	bool _win32TranslateInputEvent( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );
	bool _win32TranslateInputEventKeyboard( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );
	bool _win32TranslateInputEventMouse( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );
	bool _win32TranslateInputEventTouch( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );
	KeyCode _win32GetSysKeyCode( WPARAM pWparam );
	Bitmask<MouseButtonFlagBits> _win32GetMouseButtonStateMask( WPARAM pWparam );
	LRESULT __stdcall _win32EventSourceProxyEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );


	bool nativeEventTranslate( EventController & pEventController, const NativeEvent & pNativeEvent, EventObject & pOutEvent )
	{
	    if( pNativeEvent.message >= WM_KEYFIRST && pNativeEvent.message <= WM_TOUCH )
	    {
	        if( _win32TranslateInputEvent( pEventController, pNativeEvent, pOutEvent ) )
	        {
	            return true;
	        }
	    }
	    else
	    {
	        if( _win32TranslateAppOrWindowEvent( pEventController, pNativeEvent, pOutEvent ) )
	        {
	            return true;
	        }
	    }

	    return false;
	}

	void nativeEnableWindowEventSupport( Window & pWindow, EventController & pEventController )
	{
	    if( !pEventController.mPrivate->windowManager )
	    {
	        pEventController.mPrivate->windowManager = pWindow.mWindowManager;
	    }
	    
	    Win32WindowEventState * winEventState = new Win32WindowEventState();
	    winEventState->windowObject = &pWindow;
	    winEventState->eventController = &pEventController;

	    HWND windowHwnd = pWindow.mPrivate->nativeDataPriv.hwnd;

	    auto winEventStateAddress = reinterpret_cast<LONG_PTR>( winEventState );
	    ::SetWindowLongPtrA( windowHwnd, GWLP_USERDATA, winEventStateAddress );

        auto wndEventProcAddress = reinterpret_cast<LONG_PTR>( _win32EventSourceProxyEventProc );
        ::SetWindowLongPtrA( windowHwnd, GWLP_WNDPROC, wndEventProcAddress );

        // Trigger the update of the window to ensure changes are visible.
        ::SetWindowPos( windowHwnd,
                        nullptr,
                        0,
                        0,
                        0,
                        0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
	}


	bool _win32TranslateAppOrWindowEvent( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
	{
	    switch ( pMSG.message )
	    {
	        case WM_SHOWWINDOW:
	        {
	            auto & eWindowUpdateVisibility = pOutEvent.eWindowUpdateVisibility;
	            eWindowUpdateVisibility.eventCode = E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY;
	            eWindowUpdateVisibility.newVisibilityState = ( pMSG.wParam != FALSE );
	            eWindowUpdateVisibility.sourceWindow = pEventController.mPrivate->windowManager->findWindow( [&pMSG]( Window & pWindow ) -> bool {
	                return pWindow.mNativeData->hwnd == pMSG.hwnd;
	            });
	            break;
	        }
	        case WM_SIZE:
	        {
	            auto & eWindowUpdateResize = pOutEvent.eWindowUpdateResize;
	            eWindowUpdateResize.eventCode = E_EVENT_CODE_WINDOW_UPDATE_RESIZE;
	            eWindowUpdateResize.newSize.x = LOWORD( pMSG.lParam );
	            eWindowUpdateResize.newSize.y = HIWORD( pMSG.lParam );
	            eWindowUpdateResize.sourceWindow = pEventController.mPrivate->windowManager->findWindow( [&pMSG]( Window & pWindow ) -> bool {
	                return pWindow.mNativeData->hwnd == pMSG.hwnd;
	            });
	            break;
	        }
            case WM_CLOSE:
	        {
	            auto & eWindowUpdateClose = pOutEvent.eWindowUpdateClose;
	            eWindowUpdateClose.eventCode = E_EVENT_CODE_WINDOW_UPDATE_CLOSE;
	            eWindowUpdateClose.sourceWindow = pEventController.mPrivate->windowManager->findWindow( [&pMSG]( Window & pWindow ) -> bool {
	                return pWindow.mNativeData->hwnd == pMSG.hwnd;
	            });
	            break;
	        }
	        case WM_QUIT:
	        {
	            auto & eAppActivityQuit = pOutEvent.eAppActivityQuit;
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

	bool _win32TranslateInputEvent( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
	{
	    if( ( pMSG.message >= WM_KEYFIRST ) && ( pMSG.message <= WM_KEYLAST ) )
	    {
	        return _win32TranslateInputEventKeyboard( pEventController, pMSG, pOutEvent );
	    }
	    else if( ( pMSG.message >= WM_MOUSEFIRST ) && ( pMSG.message <= WM_MOUSELAST ) )
	    {
	        return _win32TranslateInputEventMouse( pEventController, pMSG, pOutEvent );
	    }
	#if defined( WM_TOUCH )
	    else if( pMSG.message == WM_TOUCH )
	    {
	        return _win32TranslateInputEventTouch( pEventController, pMSG, pOutEvent );
	    }
	#endif
	    else
	    {
	        return false;
	    }
	}

	bool _win32TranslateInputEventKeyboard( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
	{
	    const auto & inputState = pEventController.mPrivate->inputState;

	    switch( pMSG.message )
	    {
	        case WM_KEYDOWN:
	        {
	            auto & eInputKeyboardKey = pOutEvent.eInputKeyboardKey;
	            eInputKeyboardKey.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
	            eInputKeyboardKey.keyboardState = &( inputState.keyboardState );
	            eInputKeyboardKey.keyAction = KeyActionType::Press;
	            eInputKeyboardKey.keyCode = _win32GetSysKeyCode( pMSG.wParam );
	            break;
	        }
	        case WM_KEYUP:
	        {
	            auto & eInputKeyboardKey = pOutEvent.eInputKeyboardKey;
	            eInputKeyboardKey.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
	            eInputKeyboardKey.keyboardState = &( inputState.keyboardState );
	            eInputKeyboardKey.keyAction = KeyActionType::Release;
	            eInputKeyboardKey.keyCode = _win32GetSysKeyCode( pMSG.wParam );
	            break;
	        }
	        default:
	        {
	            return false;
	        }
	    }

	    return true;
	}

	bool _win32TranslateInputEventMouse( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
	{
	    const auto cursorPos = math::Vec2i32{
	        GET_X_LPARAM( pMSG.lParam ),
	        GET_Y_LPARAM( pMSG.lParam )
	    };

	    auto & inputState = pEventController.mPrivate->inputState;

	    if ( inputState.mouseLastRegPos == CX_EVENT_MOUSE_POS_INVALID )
	    {
	        inputState.mouseLastRegPos = cursorPos;
	    }

	    switch( pMSG.message )
	    {
	        case WM_MOUSEMOVE:
	        {
	            auto & eInputMouseMove = pOutEvent.eInputMouseMove;
	            eInputMouseMove.eventCode = E_EVENT_CODE_INPUT_MOUSE_MOVE;
	            eInputMouseMove.cursorPos = cursorPos;
	            eInputMouseMove.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            eInputMouseMove.movementDelta = cursorPos - inputState.mouseLastRegPos;
	            inputState.mouseLastRegPos = cursorPos;
	            break;
	        }
	        case WM_LBUTTONDOWN:
	        {
	            auto & eInputMouseButton = pOutEvent.eInputMouseButton;
	            eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
	            eInputMouseButton.cursorPos = cursorPos;
	            eInputMouseButton.buttonAction = MouseButtonActionType::Click;
	            eInputMouseButton.buttonID = MouseButtonID::Left;
	            eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            break;
	        }
	        case WM_LBUTTONUP:
	        {
	            auto & eInputMouseButton = pOutEvent.eInputMouseButton;
	            eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
	            eInputMouseButton.cursorPos = cursorPos;
	            eInputMouseButton.buttonAction = MouseButtonActionType::Release;
	            eInputMouseButton.buttonID = MouseButtonID::Left;
	            eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            break;
	        }
	        case WM_LBUTTONDBLCLK:
	        {
	            pOutEvent.commonData.eventCode = E_EVENT_CODE_UNDEFINED;
	            break;
	        }
	        case WM_RBUTTONDOWN:
	        {
	            auto & eInputMouseButton = pOutEvent.eInputMouseButton;
	            eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
	            eInputMouseButton.cursorPos = cursorPos;
	            eInputMouseButton.buttonAction = MouseButtonActionType::Click;
	            eInputMouseButton.buttonID = MouseButtonID::Right;
	            eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            break;
	        }
	        case WM_RBUTTONUP:
	        {
	            auto & eInputMouseButton = pOutEvent.eInputMouseButton;
	            eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
	            eInputMouseButton.cursorPos = cursorPos;
	            eInputMouseButton.buttonAction = MouseButtonActionType::Release;
	            eInputMouseButton.buttonID = MouseButtonID::Right;
	            eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            break;
	        }
	        case WM_RBUTTONDBLCLK:
	        {
	            pOutEvent.commonData.eventCode = E_EVENT_CODE_UNDEFINED;
	            break;
	        }
	        case WM_MBUTTONDOWN:
	        {
	            auto & eInputMouseButton = pOutEvent.eInputMouseButton;
	            eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
	            eInputMouseButton.cursorPos = cursorPos;
	            eInputMouseButton.buttonAction = MouseButtonActionType::Click;
	            eInputMouseButton.buttonID = MouseButtonID::Middle;
	            eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            break;
	        }
	        case WM_MBUTTONUP:
	        {
	            auto & eInputMouseButton = pOutEvent.eInputMouseButton;
	            eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
	            eInputMouseButton.cursorPos = cursorPos;
	            eInputMouseButton.buttonAction = MouseButtonActionType::Release;
	            eInputMouseButton.buttonID = MouseButtonID::Middle;
	            eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            break;
	        }
	        case WM_MBUTTONDBLCLK:
	        {
	            pOutEvent.commonData.eventCode = E_EVENT_CODE_UNDEFINED;
	            break;
	        }
	        case WM_MOUSEWHEEL:
	        {
	            auto & eInputMouseScroll = pOutEvent.eInputMouseScroll;
	            pOutEvent.code = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
	            eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
	            eInputMouseScroll.scrollDelta.x = 0;
	            eInputMouseScroll.scrollDelta.y = static_cast<int32>( GET_WHEEL_DELTA_WPARAM( pMSG.wParam ) );
	            break;
	        }
	        case WM_XBUTTONDOWN:
	        {
	            auto & eInputMouseButton = pOutEvent.eInputMouseButton;
	            eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
	            eInputMouseButton.cursorPos = cursorPos;
	            eInputMouseButton.buttonAction = MouseButtonActionType::Click;
	            eInputMouseButton.buttonID = ( GET_XBUTTON_WPARAM( pMSG.wParam ) == XBUTTON1 ) ? MouseButtonID::XB1 : MouseButtonID::XB2;
	            eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            break;
	        }
	        case WM_XBUTTONUP:
	        {
	            auto & eInputMouseButton = pOutEvent.eInputMouseButton;
	            eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
	            eInputMouseButton.cursorPos = cursorPos;
	            eInputMouseButton.buttonAction = MouseButtonActionType::Release;
	            eInputMouseButton.buttonID = ( GET_XBUTTON_WPARAM( pMSG.wParam ) == XBUTTON1 ) ? MouseButtonID::XB1 : MouseButtonID::XB2;
	            eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
	            break;
	        }
	        case WM_XBUTTONDBLCLK:
	        {
	            pOutEvent.commonData.eventCode = E_EVENT_CODE_UNDEFINED;
	            break;
	        }
	        case WM_MOUSEHWHEEL:
	        {
	            auto & eInputMouseScroll = pOutEvent.eInputMouseScroll;
	            eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
	            eInputMouseScroll.scrollDelta.x = static_cast<int32>( GET_WHEEL_DELTA_WPARAM( pMSG.wParam ) );
	            eInputMouseScroll.scrollDelta.y = 0;
	            break;
	        }
	    }

	    return pOutEvent.commonData.eventCode != E_EVENT_CODE_UNDEFINED;
	}

	bool _win32TranslateInputEventTouch( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
    {
	    return false;
    }


	static const KeyCode sASCIIKeyCodeMap_08_7B[] =
	{
		/* 0x0008 */ KeyCode::Backspace,
		/* 0x0009 */ KeyCode::Tab,
		/* 0x000A */ KeyCode::Unknown,
		/* 0x000B */ KeyCode::Unknown,
		/* 0x000C */ KeyCode::Unknown,
		/* 0x000D */ KeyCode::Enter,
		/* 0x000E */ KeyCode::Unknown,
		/* 0x000F */ KeyCode::Unknown,
		/* 0x0010 */ KeyCode::ShiftLeft,
		/* 0x0011 */ KeyCode::CtrlLeft,
		/* 0x0012 */ KeyCode::AltLeft,
		/* 0x0013 */ KeyCode::Unknown,
		/* 0x0014 */ KeyCode::CapsLock,
		/* 0x0015 */ KeyCode::Unknown,
		/* 0x0016 */ KeyCode::Unknown,
		/* 0x0017 */ KeyCode::Unknown,
		/* 0x0018 */ KeyCode::Unknown,
		/* 0x0019 */ KeyCode::Unknown,
		/* 0x001A */ KeyCode::Unknown,
		/* 0x001B */ KeyCode::Escape,
		/* 0x001C */ KeyCode::Unknown,
		/* 0x001D */ KeyCode::Unknown,
		/* 0x001E */ KeyCode::Unknown,
		/* 0x001F */ KeyCode::Unknown,
		/* 0x0020 */ KeyCode::Space,
		/* 0x0021 */ KeyCode::PageUp,
		/* 0x0022 */ KeyCode::PageDown,
		/* 0x0023 */ KeyCode::End,
		/* 0x0024 */ KeyCode::Home,
		/* 0x0025 */ KeyCode::ArrowLeft,
		/* 0x0026 */ KeyCode::ArrowUp,
		/* 0x0027 */ KeyCode::ArrowRight,
		/* 0x0028 */ KeyCode::ArrowDown,
		/* 0x0029 */ KeyCode::Unknown,
		/* 0x002A */ KeyCode::Unknown,
		/* 0x002B */ KeyCode::Unknown,
		/* 0x002C */ KeyCode::Unknown,
		/* 0x002D */ KeyCode::Insert,
		/* 0x002E */ KeyCode::Delete,
		/* 0x002F */ KeyCode::Unknown,
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
		/* 0x003A */ KeyCode::Unknown,
		/* 0x003B */ KeyCode::Unknown,
		/* 0x003C */ KeyCode::Unknown,
		/* 0x003D */ KeyCode::Unknown,
		/* 0x003E */ KeyCode::Unknown,
		/* 0x003F */ KeyCode::Unknown,
		/* 0x0040 */ KeyCode::Unknown,
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
		/* 0x005B */ KeyCode::Unknown,
		/* 0x005C */ KeyCode::Unknown,
		/* 0x005D */ KeyCode::Unknown,
		/* 0x005E */ KeyCode::Unknown,
		/* 0x005F */ KeyCode::Unknown,
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

	static const KeyCode sASCIIKeyCodeMap_A0_A5[] =
	{
		/* 0x00A0 */ KeyCode::ShiftLeft,
		/* 0x00A1 */ KeyCode::ShiftRight,
		/* 0x00A2 */ KeyCode::CtrlLeft,
		/* 0x00A3 */ KeyCode::CtrlRight,
		/* 0x00A4 */ KeyCode::AltLeft,
		/* 0x00A5 */ KeyCode::AltRight,
	};

	KeyCode _win32GetSysKeyCode( WPARAM pWparam )
	{
		if ( ( pWparam >= 0x0008 ) && ( pWparam <= 0x007B ) )
		{
			auto baseIndex = pWparam - 0x0008;
			return sASCIIKeyCodeMap_08_7B[baseIndex];
		}

		if ( ( pWparam >= 0x00A0 ) && ( pWparam <= 0x00A5 ) )
		{
			auto baseIndex = pWparam - 0x00A0;
			return sASCIIKeyCodeMap_A0_A5[baseIndex];
		}

		return KeyCode::Unknown;
	}

	Bitmask<MouseButtonFlagBits> _win32GetMouseButtonStateMask( WPARAM pWparam )
	{
		return static_cast<MouseButtonFlagBits>( static_cast<uint32>( pWparam ) & SYS_MOUSE_BUTTON_FLAG_ALL_BIT );
	}

	LRESULT __stdcall _win32EventSourceProxyEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam )
	{
	    LONG_PTR windowUserData = ::GetWindowLongPtrA( pHWND, GWLP_USERDATA );

	    if( windowUserData != 0 )
	    {
	        auto * winEventState = reinterpret_cast<Win32WindowEventState *>( windowUserData );

	        MSG nativeEvent;
	        nativeEvent.hwnd = pHWND;
	        nativeEvent.message = pMessage;
	        nativeEvent.wParam = pWparam;
	        nativeEvent.lParam = pLparam;

	        nativeEventDispatch( *( winEventState->eventController ), nativeEvent );

	        if( pMessage == WM_NCDESTROY )
	        {
	            delete winEventState;

	            ::SetWindowLongPtrA( pHWND, GWLP_USERDATA, 0 );
	        }
	    }

	    return ::DefWindowProcA( pHWND, pMessage, pWparam, pLparam );
	}

} // namespace ts3::system
#endif
