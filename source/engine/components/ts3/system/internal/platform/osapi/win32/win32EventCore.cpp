
#include "win32EventCore.h"
#include <ts3/system/internal/eventCorePrivate.h>
#include <ts3/math/vectorOps.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    namespace platform
    {

        LRESULT __stdcall _win32EventSourceProxyEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam );

        bool _win32TranslateAppOrWindowEvent( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );

        bool _win32TranslateInputEvent( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );

        bool _win32TranslateInputEventKeyboard( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );

        bool _win32TranslateInputEventMouse( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );

        bool _win32TranslateInputEventTouch( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );

        EWindowVisibilityState _win32TranslateWindowVisibility( WPARAM pWparam );

        EKeyCode _win32GetSysKeyCode( WPARAM pWparam );

        Bitmask<EMouseButtonFlagBits> _win32GetMouseButtonStateMask( WPARAM pWparam );

    }
    
    
    Win32EventController::Win32EventController( SysContextHandle pSysContext )
    : Win32NativeObject( std::move( pSysContext ) )
    {}

    Win32EventController::~Win32EventController() noexcept
    {}

    void Win32EventController::_nativeRegisterEventSource( EventSource & pEventSource )
    {
        auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::Win32EventSourceNativeData>();
        ts3DebugAssert( eventSourceNativeData != nullptr );

        auto * win32EventSourceState = new platform::Win32EventSourceState();
        win32EventSourceState->eventController = this;
        win32EventSourceState->savedEventCallback = ::GetWindowLongPtrA( eventSourceNativeData->hwnd, GWLP_WNDPROC );
        win32EventSourceState->savedEventCallbackUserData = ::GetWindowLongPtrA( eventSourceNativeData->hwnd, GWLP_USERDATA );

        auto win32EventSourceStateAddress = reinterpret_cast<LONG_PTR>( win32EventSourceState );
        ::SetWindowLongPtrA( eventSourceNativeData->hwnd, GWLP_USERDATA, win32EventSourceStateAddress );

        auto eventSourceProcAddress = reinterpret_cast<LONG_PTR>( platform::_win32EventSourceProxyEventProc );
        ::SetWindowLongPtrA( eventSourceNativeData->hwnd, GWLP_WNDPROC, eventSourceProcAddress );

        // Trigger the update of the window to ensure changes are visible.
        ::SetWindowPos( eventSourceNativeData->hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
    }

    void Win32EventController::_nativeUnregisterEventSource( EventSource & pEventSource )
    {
		LONG_PTR windowUserData = ::GetWindowLongPtrA( pHWND, GWLP_USERDATA );
		auto * win32EventSourceState = reinterpret_cast<Win32EventSourceState *>( windowUserData );
        if( auto * win32EventSourceState = pEventSource.getEventSourcePlatformDataAs<platform::Win32EventSourceState>() )
        {
            if( auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::Win32EventSourceNativeData>() )
            {
                ::SetWindowLongPtrA( eventSourceNativeData->hwnd, GWLP_WNDPROC, win32EventSourceState->savedEventCallback );

                ::SetWindowLongPtrA( eventSourceNativeData->hwnd, GWLP_USERDATA, win32EventSourceState->savedEventCallbackUserData );

                // Trigger the update of the window to ensure changes are visible.
				::SetWindowPos( eventSourceNativeData->hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
            }

            resetEventSourcePlatformData( pEventSource );
        }
    }

    bool Win32EventController::_nativeUpdateSysQueue()
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

    bool Win32EventController::_nativeUpdateSysQueueWait()
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


    namespace platform
    {

        bool nativeEventTranslate( EventController & pEventController, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
        {
        	auto * win32EventController = pEventController.queryInterface<Win32EventController>();

        	return win32TranslateEvent( *win32EventController, pNativeEvent, pOutEvent );
        }

		EventSource * win32FindEventSourceByHWND( Win32EventController & pEventController, HWND pHWND )
		{
        	auto * eventSource = pEventController.findEventSource( [pHWND]( const EventSource & pEventSource ) -> bool {
				const auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::Win32EventSourceNativeData>();
				return eventSourceNativeData->hwnd == pHWND;
			});
			return eventSource;
		}

		bool win32TranslateEvent( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
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

        LRESULT __stdcall win32DefaultWindowEventCallback( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam )
        {
			switch( pMessage )
			{
				case WM_NCCREATE:
				{
					LONG_PTR windowUserData = ::GetWindowLongPtrA( pHWND, GWLP_USERDATA );
					if( windowUserData != 0 )
					{
						// TODO: Log this!
						ts3DebugInterrupt();
					}
					auto * win32EventSourceState = new platform::Win32EventSourceState();
					auto win32EventSourceStateAddress = reinterpret_cast<LONG_PTR>( win32EventSourceState );
					::SetWindowLongPtrA( pHWND, GWLP_USERDATA, win32EventSourceStateAddress );
					break;
				}
				case WM_NCDESTROY:
				{
					LONG_PTR windowUserData = ::GetWindowLongPtrA( pHWND, GWLP_USERDATA );
					if( windowUserData != 0 )
					{
						auto * win32EventSourceState = reinterpret_cast<platform::Win32EventSourceState *>( windowUserData );
						delete win32EventSourceState;
						::SetWindowLongPtrA( pHWND, GWLP_USERDATA, 0 );
					}
					break;
				}
			}
            return ::DefWindowProcA( pHWND, pMessage, pWparam, pLparam );
        }

        LRESULT __stdcall _win32EventSourceProxyEventProc( HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam )
        {
            // The UserData value is a pointer to the internal EventState of the current window.
            // It is created before this callback gets injected and released when NCDESTROY message arrives.
            LONG_PTR windowUserData = ::GetWindowLongPtrA( pHWND, GWLP_USERDATA );

            if( windowUserData != 0 )
            {
                // This our state.
                // TODO: Some validation might be useful to check if this callback is used correctly.
                auto * win32EventSourceState = reinterpret_cast<Win32EventSourceState *>( windowUserData );

                Win32NativeEvent nativeEvent;
                nativeEvent.hwnd = pHWND;
                nativeEvent.message = pMessage;
                nativeEvent.wParam = pWparam;
                nativeEvent.lParam = pLparam;

                nativeEventDispatch( *( win32EventSourceState->eventController ), nativeEvent );
            }

            return ::DefWindowProcA( pHWND, pMessage, pWparam, pLparam );
        }

        bool _win32TranslateAppOrWindowEvent( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
        {
            switch ( pMSG.message )
            {
                case WM_SHOWWINDOW:
                {
                    auto * eventSource = win32FindEventSourceByHWND( pEventController, pMSG.hwnd );
                    auto & eWindowUpdateVisibility = pOutEvent.eWindowUpdateVisibility;
                    eWindowUpdateVisibility.eventCode = E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY;
                    eWindowUpdateVisibility.newVisibilityState = _win32TranslateWindowVisibility( pMSG.wParam );
                    eWindowUpdateVisibility.eventSource = eventSource;
                    break;
                }
                case WM_SIZE:
                {
                    auto * eventSource = win32FindEventSourceByHWND( pEventController, pMSG.hwnd );
                    auto & eWindowUpdateResize = pOutEvent.eWindowUpdateResize;
                    eWindowUpdateResize.eventCode = E_EVENT_CODE_WINDOW_UPDATE_RESIZE;
                    eWindowUpdateResize.newSize.x = LOWORD( pMSG.lParam );
                    eWindowUpdateResize.newSize.y = HIWORD( pMSG.lParam );
                    eWindowUpdateResize.eventSource = eventSource;
                    break;
                }
                case WM_CLOSE:
                {
                    auto * eventSource = win32FindEventSourceByHWND( pEventController, pMSG.hwnd );
                    auto & eWindowUpdateDestroy = pOutEvent.eWindowUpdateDestroy;
                    eWindowUpdateDestroy.eventCode = E_EVENT_CODE_WINDOW_UPDATE_DESTROY;
                    eWindowUpdateDestroy.eventSource = eventSource;
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

        bool _win32TranslateInputEvent( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
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

        bool _win32TranslateInputEventKeyboard( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
        {
            auto & inputKeyboardState = pEventController.getEventDispatcherInputState().inputKeyboardState;

            switch( pMSG.message )
            {
                case WM_KEYDOWN:
                {
                    auto & eInputKeyboardKey = pOutEvent.eInputKeyboardKey;
                    eInputKeyboardKey.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
                    eInputKeyboardKey.inputKeyboardState = &inputKeyboardState;
                    eInputKeyboardKey.keyAction = EKeyActionType::Press;
                    eInputKeyboardKey.keyCode = _win32GetSysKeyCode( pMSG.wParam );
                    break;
                }
                case WM_KEYUP:
                {
                    auto & eInputKeyboardKey = pOutEvent.eInputKeyboardKey;
                    eInputKeyboardKey.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
                    eInputKeyboardKey.inputKeyboardState = &inputKeyboardState;
                    eInputKeyboardKey.keyAction = EKeyActionType::Release;
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

        bool _win32TranslateInputEventMouse( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
        {
            const auto cursorPos = math::Vec2i32{
                GET_X_LPARAM( pMSG.lParam ),
                GET_Y_LPARAM( pMSG.lParam )
            };

            auto & inputMouseState = pEventController.getEventDispatcherInputState().inputMouseState;

            if ( inputMouseState.lastCursorPos == CX_EVENT_MOUSE_POS_INVALID )
            {
                inputMouseState.lastCursorPos = cursorPos;
            }

            switch( pMSG.message )
            {
                case WM_MOUSEMOVE:
                {
                    auto & eInputMouseMove = pOutEvent.eInputMouseMove;
                    eInputMouseMove.eventCode = E_EVENT_CODE_INPUT_MOUSE_MOVE;
                    eInputMouseMove.cursorPos = cursorPos;
                    eInputMouseMove.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    eInputMouseMove.movementDelta = cursorPos - inputMouseState.lastCursorPos;
                    eInputMouseMove.inputMouseState = &inputMouseState;
                    inputMouseState.lastCursorPos = cursorPos;
                    break;
                }
                case WM_LBUTTONDOWN:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
                    eInputMouseButton.buttonID = EMouseButtonID::Left;
                    eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    break;
                }
                case WM_LBUTTONUP:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
                    eInputMouseButton.buttonID = EMouseButtonID::Left;
                    eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    break;
                }
                case WM_LBUTTONDBLCLK:
                {
                    pOutEvent.code = E_EVENT_CODE_UNDEFINED;
                    break;
                }
                case WM_RBUTTONDOWN:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
                    eInputMouseButton.buttonID = EMouseButtonID::Right;
                    eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    break;
                }
                case WM_RBUTTONUP:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
                    eInputMouseButton.buttonID = EMouseButtonID::Right;
                    eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    break;
                }
                case WM_RBUTTONDBLCLK:
                {
                    pOutEvent.code = E_EVENT_CODE_UNDEFINED;
                    break;
                }
                case WM_MBUTTONDOWN:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
                    eInputMouseButton.buttonID = EMouseButtonID::Middle;
                    eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    break;
                }
                case WM_MBUTTONUP:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
                    eInputMouseButton.buttonID = EMouseButtonID::Middle;
                    eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    break;
                }
                case WM_MBUTTONDBLCLK:
                {
                    pOutEvent.code = E_EVENT_CODE_UNDEFINED;
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
                    eInputMouseButton.buttonAction = EMouseButtonActionType::Click;
                    eInputMouseButton.buttonID = ( GET_XBUTTON_WPARAM( pMSG.wParam ) == XBUTTON1 ) ? EMouseButtonID::XB1 : EMouseButtonID::XB2;
                    eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    break;
                }
                case WM_XBUTTONUP:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = EMouseButtonActionType::Release;
                    eInputMouseButton.buttonID = ( GET_XBUTTON_WPARAM( pMSG.wParam ) == XBUTTON1 ) ? EMouseButtonID::XB1 : EMouseButtonID::XB2;
                    eInputMouseButton.buttonStateMask = _win32GetMouseButtonStateMask( pMSG.wParam );
                    break;
                }
                case WM_XBUTTONDBLCLK:
                {
                    pOutEvent.code = E_EVENT_CODE_UNDEFINED;
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

            return pOutEvent.code != E_EVENT_CODE_UNDEFINED;
        }

        bool _win32TranslateInputEventTouch( Win32EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent )
        {
            return false;
        }

        EWindowVisibilityState _win32TranslateWindowVisibility( WPARAM pWparam )
        {
        	if( pWparam != FALSE )
        	{
        		return EWindowVisibilityState::Visible;
        	}
        	else
        	{
        		return EWindowVisibilityState::Hidden;
        	}
        }

        static const EKeyCode sASCIIKeyCodeMap_08_7B[] =
        {
            /* 0x0008 */ EKeyCode::Backspace,
            /* 0x0009 */ EKeyCode::Tab,
            /* 0x000A */ EKeyCode::Unknown,
            /* 0x000B */ EKeyCode::Unknown,
            /* 0x000C */ EKeyCode::Unknown,
            /* 0x000D */ EKeyCode::Enter,
            /* 0x000E */ EKeyCode::Unknown,
            /* 0x000F */ EKeyCode::Unknown,
            /* 0x0010 */ EKeyCode::ShiftLeft,
            /* 0x0011 */ EKeyCode::CtrlLeft,
            /* 0x0012 */ EKeyCode::AltLeft,
            /* 0x0013 */ EKeyCode::Unknown,
            /* 0x0014 */ EKeyCode::CapsLock,
            /* 0x0015 */ EKeyCode::Unknown,
            /* 0x0016 */ EKeyCode::Unknown,
            /* 0x0017 */ EKeyCode::Unknown,
            /* 0x0018 */ EKeyCode::Unknown,
            /* 0x0019 */ EKeyCode::Unknown,
            /* 0x001A */ EKeyCode::Unknown,
            /* 0x001B */ EKeyCode::Escape,
            /* 0x001C */ EKeyCode::Unknown,
            /* 0x001D */ EKeyCode::Unknown,
            /* 0x001E */ EKeyCode::Unknown,
            /* 0x001F */ EKeyCode::Unknown,
            /* 0x0020 */ EKeyCode::Space,
            /* 0x0021 */ EKeyCode::PageUp,
            /* 0x0022 */ EKeyCode::PageDown,
            /* 0x0023 */ EKeyCode::End,
            /* 0x0024 */ EKeyCode::Home,
            /* 0x0025 */ EKeyCode::ArrowLeft,
            /* 0x0026 */ EKeyCode::ArrowUp,
            /* 0x0027 */ EKeyCode::ArrowRight,
            /* 0x0028 */ EKeyCode::ArrowDown,
            /* 0x0029 */ EKeyCode::Unknown,
            /* 0x002A */ EKeyCode::Unknown,
            /* 0x002B */ EKeyCode::Unknown,
            /* 0x002C */ EKeyCode::Unknown,
            /* 0x002D */ EKeyCode::Insert,
            /* 0x002E */ EKeyCode::Delete,
            /* 0x002F */ EKeyCode::Unknown,
            /* 0x0030 */ EKeyCode::Num0,
            /* 0x0031 */ EKeyCode::Num1,
            /* 0x0032 */ EKeyCode::Num2,
            /* 0x0033 */ EKeyCode::Num3,
            /* 0x0034 */ EKeyCode::Num4,
            /* 0x0035 */ EKeyCode::Num5,
            /* 0x0036 */ EKeyCode::Num6,
            /* 0x0037 */ EKeyCode::Num7,
            /* 0x0038 */ EKeyCode::Num8,
            /* 0x0039 */ EKeyCode::Num9,
            /* 0x003A */ EKeyCode::Unknown,
            /* 0x003B */ EKeyCode::Unknown,
            /* 0x003C */ EKeyCode::Unknown,
            /* 0x003D */ EKeyCode::Unknown,
            /* 0x003E */ EKeyCode::Unknown,
            /* 0x003F */ EKeyCode::Unknown,
            /* 0x0040 */ EKeyCode::Unknown,
            /* 0x0041 */ EKeyCode::CharA,
            /* 0x0042 */ EKeyCode::CharB,
            /* 0x0043 */ EKeyCode::CharC,
            /* 0x0044 */ EKeyCode::CharD,
            /* 0x0045 */ EKeyCode::CharE,
            /* 0x0046 */ EKeyCode::CharF,
            /* 0x0047 */ EKeyCode::CharG,
            /* 0x0048 */ EKeyCode::CharH,
            /* 0x0049 */ EKeyCode::CharI,
            /* 0x004A */ EKeyCode::CharJ,
            /* 0x004B */ EKeyCode::CharK,
            /* 0x004C */ EKeyCode::CharL,
            /* 0x004D */ EKeyCode::CharM,
            /* 0x004E */ EKeyCode::CharN,
            /* 0x004F */ EKeyCode::CharO,
            /* 0x0050 */ EKeyCode::CharP,
            /* 0x0051 */ EKeyCode::CharQ,
            /* 0x0052 */ EKeyCode::CharR,
            /* 0x0053 */ EKeyCode::CharS,
            /* 0x0054 */ EKeyCode::CharT,
            /* 0x0055 */ EKeyCode::CharU,
            /* 0x0056 */ EKeyCode::CharV,
            /* 0x0057 */ EKeyCode::CharW,
            /* 0x0058 */ EKeyCode::CharX,
            /* 0x0059 */ EKeyCode::CharY,
            /* 0x005A */ EKeyCode::CharZ,
            /* 0x005B */ EKeyCode::Unknown,
            /* 0x005C */ EKeyCode::Unknown,
            /* 0x005D */ EKeyCode::Unknown,
            /* 0x005E */ EKeyCode::Unknown,
            /* 0x005F */ EKeyCode::Unknown,
            /* 0x0060 */ EKeyCode::NumPad0,
            /* 0x0061 */ EKeyCode::NumPad1,
            /* 0x0062 */ EKeyCode::NumPad2,
            /* 0x0063 */ EKeyCode::NumPad3,
            /* 0x0064 */ EKeyCode::NumPad4,
            /* 0x0065 */ EKeyCode::NumPad5,
            /* 0x0066 */ EKeyCode::NumPad6,
            /* 0x0067 */ EKeyCode::NumPad7,
            /* 0x0068 */ EKeyCode::NumPad8,
            /* 0x0069 */ EKeyCode::NumPad9,
            /* 0x006A */ EKeyCode::NumPadMul,
            /* 0x006B */ EKeyCode::NumPadAdd,
            /* 0x006C */ EKeyCode::Unknown,
            /* 0x006D */ EKeyCode::NumPadSub,
            /* 0x006E */ EKeyCode::NumPadDot,
            /* 0x006F */ EKeyCode::NumPadDiv,
            /* 0x0070 */ EKeyCode::F1,
            /* 0x0071 */ EKeyCode::F2,
            /* 0x0072 */ EKeyCode::F3,
            /* 0x0073 */ EKeyCode::F4,
            /* 0x0074 */ EKeyCode::F5,
            /* 0x0075 */ EKeyCode::F6,
            /* 0x0076 */ EKeyCode::F7,
            /* 0x0077 */ EKeyCode::F8,
            /* 0x0078 */ EKeyCode::F9,
            /* 0x0079 */ EKeyCode::F10,
            /* 0x007A */ EKeyCode::F11,
            /* 0x007B */ EKeyCode::F12
        };

        static const EKeyCode sASCIIKeyCodeMap_A0_A5[] =
        {
            /* 0x00A0 */ EKeyCode::ShiftLeft,
            /* 0x00A1 */ EKeyCode::ShiftRight,
            /* 0x00A2 */ EKeyCode::CtrlLeft,
            /* 0x00A3 */ EKeyCode::CtrlRight,
            /* 0x00A4 */ EKeyCode::AltLeft,
            /* 0x00A5 */ EKeyCode::AltRight,
        };

        EKeyCode _win32GetSysKeyCode( WPARAM pWparam )
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

            return EKeyCode::Unknown;
        }

        Bitmask<EMouseButtonFlagBits> _win32GetMouseButtonStateMask( WPARAM pWparam )
        {
            return static_cast<EMouseButtonFlagBits>( static_cast<uint32>( pWparam ) & E_MOUSE_BUTTON_FLAG_ALL_BIT );
        }

    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
