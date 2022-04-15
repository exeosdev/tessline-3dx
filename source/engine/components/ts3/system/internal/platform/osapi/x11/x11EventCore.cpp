
#include "x11EventCore.h"
#include <ts3/system/internal/eventCorePrivate.h>
#include <ts3/math/vectorOps.h>
#include <X11/keysym.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

    namespace platform
    {

        bool _x11TranslateGenericEvent( X11EventController & pEventController, const XEvent & pXEvent, EventObject & pOutEvent );
        
        bool _x11TranslateInputEvent( X11EventController & pEventController, const XEvent & pXEvent, EventObject & pOutEvent );
        
        bool _x11TranslateInputEventMouseButton( X11EventController & pEventController,
                                                 const XEvent & pXEvent,
                                                 EventObject & pOutEvent,
                                                 EMouseButtonActionType pButtonAction );
        
        bool _x11TranslateSystemEvent( X11EventController & pEventController, const XEvent & pXEvent, EventObject & pOutEvent );
        
        EKeyCode _x11GetSysKeyCode( KeySym pXkeySym );
        
        EX11MouseButtonID _x11GetMouseButtonID( uint32 pButton );
        
        Bitmask<EMouseButtonFlagBits> _x11GetMouseButtonStateMask( uint32 pState );
        
    }


    X11EventController::X11EventController( SysContextHandle pSysContext )
    : X11NativeObject( std::move( pSysContext ) )
    {}

    X11EventController::~X11EventController() noexcept
    {}

	void X11EventController::_nativeRegisterEventSource( EventSource & /* pEventSource */ )
    {}

    void X11EventController::_nativeUnregisterEventSource( EventSource & /* pEventSource */ )
    {}

    bool X11EventController::_nativeUpdateSysQueue()
    {
        auto & xSessionData = platform::x11GetXSessionData( *this );

        platform::NativeEventType x11NativeEvent;
        if( XEventsQueued( xSessionData.display, QueuedAfterReading ) > 0 )
        {
            XNextEvent( xSessionData.display, &( x11NativeEvent.xEvent ) );
            platform::nativeEventDispatch( *this, x11NativeEvent );
            return true;
        }

        return false;
    }

    bool X11EventController::_nativeUpdateSysQueueWait()
    {
        auto & xSessionData = platform::x11GetXSessionData( *this );

        platform::NativeEventType x11NativeEvent;
        if( XEventsQueued( xSessionData.display, QueuedAfterFlush ) > 0 )
        {
            XNextEvent( xSessionData.display, &( x11NativeEvent.xEvent ) );
            platform::nativeEventDispatch( *this, x11NativeEvent );
            return true;
        }

        return false;
    }


    namespace platform
    {

        bool nativeEventTranslate( EventController & pEventController, const NativeEventType & pNativeEvent, EventObject & pOutEvent )
        {
            auto * x11EventController = pEventController.queryInterface<X11EventController>();

            if( ( pNativeEvent.xEvent.type >= KeyPress ) && ( pNativeEvent.xEvent.type <= MotionNotify ) )
            {
                if( _x11TranslateInputEvent( *x11EventController, pNativeEvent.xEvent, pOutEvent ) )
                {
                    return true;
                }
            }
            else if( ( pNativeEvent.xEvent.type >= EnterNotify ) && ( pNativeEvent.xEvent.type <= MapRequest ) )
            {
                if( _x11TranslateGenericEvent( *x11EventController, pNativeEvent.xEvent, pOutEvent  ) )
                {
                    return true;
                }
            }
            else
            {
                if( _x11TranslateSystemEvent( *x11EventController, pNativeEvent.xEvent, pOutEvent  ) )
                {
                    return true;
                }
            }

            return false;
        }


        bool _x11TranslateGenericEvent( X11EventController & pEventController, const XEvent & pXEvent, EventObject & pOutEvent )
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
                {
                    auto * eventSource = pEventController.findEventSource( [&pXEvent]( const EventSource & pEventSource ) -> bool {
                        const auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::X11EventSourceNativeData>();
                        return eventSourceNativeData->windowXID == pXEvent.xdestroywindow.window;
                    });
                    auto & eWindowUpdateClose = pOutEvent.eWindowUpdateClose;
                    eWindowUpdateClose.eventCode = E_EVENT_CODE_WINDOW_UPDATE_CLOSE;
                    eWindowUpdateClose.eventSource = eventSource;
                    break;
                }
                case UnmapNotify:
                {
                    auto * eventSource = pEventController.findEventSource( [&pXEvent]( const EventSource & pEventSource ) -> bool {
                        const auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::X11EventSourceNativeData>();
                        return eventSourceNativeData->windowXID == pXEvent.xvisibility.window;
                    });
                    auto & eWindowUpdateVisibility = pOutEvent.eWindowUpdateVisibility;
                    eWindowUpdateVisibility.eventCode = E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY;
                    eWindowUpdateVisibility.newVisibilityState = false;
                    eWindowUpdateVisibility.eventSource = eventSource;
                    break;
                }
                case MapNotify:
                {
                    auto * eventSource = pEventController.findEventSource( [&pXEvent]( const EventSource & pEventSource ) -> bool {
                        const auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::X11EventSourceNativeData>();
                        return eventSourceNativeData->windowXID == pXEvent.xvisibility.window;
                    });
                    auto & eWindowUpdateVisibility = pOutEvent.eWindowUpdateVisibility;
                    eWindowUpdateVisibility.eventCode = E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY;
                    eWindowUpdateVisibility.newVisibilityState = true;
                    eWindowUpdateVisibility.eventSource = eventSource;
                    break;
                }
                case MapRequest:
                    break;
                default:
                    return false;
            }

            return true;
        }

        bool _x11TranslateInputEvent( X11EventController & pEventController, const XEvent & pXEvent, EventObject & pOutEvent )
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
                    auto & inputKeyboardState = pEventController.getEventDispatcherInputState().inputKeyboardState;

                    // XLookupKeysym requires non-const XKeyEvent parameter.
                    // Use locally copied one to keep the API right (we don't want to drop 'const for input event param).
                    auto xKey = pXEvent.xkey;
                    auto keysym = XLookupKeysym( &xKey, 0 );
                    auto keycode = _x11GetSysKeyCode( keysym );
                    auto & eventData = pOutEvent.eInputKeyboardKey;
                    eventData.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
                    eventData.inputKeyboardState = &inputKeyboardState;
                    eventData.keyCode = keycode;
                    eventData.keyAction = EKeyActionType::Press;
                    break;
                }
                case KeyRelease:
                {
                    auto & inputKeyboardState = pEventController.getEventDispatcherInputState().inputKeyboardState;

                    // XLookupKeysym requires non-const XKeyEvent parameter.
                    // Use locally copied one to keep the API right (we don't want to drop 'const for input event param).
                    auto xKey = pXEvent.xkey;
                    auto keysym = XLookupKeysym( &xKey, 0 );
                    auto keycode = _x11GetSysKeyCode( keysym );
                    auto & eventData = pOutEvent.eInputKeyboardKey;
                    eventData.eventCode = E_EVENT_CODE_INPUT_KEYBOARD_KEY;
                    eventData.inputKeyboardState = &inputKeyboardState;
                    eventData.keyCode = keycode;
                    eventData.keyAction = EKeyActionType::Release;

                    break;
                }
                case ButtonPress:
                {
                    _x11TranslateInputEventMouseButton( pEventController, pXEvent, pOutEvent, EMouseButtonActionType::Click );

                    break;
                }
                case ButtonRelease:
                {
                    _x11TranslateInputEventMouseButton( pEventController, pXEvent, pOutEvent, EMouseButtonActionType::Release );

                    break;
                }
                case MotionNotify:
                {
                    auto & inputMouseState = pEventController.getEventDispatcherInputState().inputMouseState;
                    const math::Vec2i32 cursorPos { pXEvent.xmotion.x, pXEvent.xmotion.y };

                    if ( inputMouseState.lastCursorPos == CX_EVENT_MOUSE_POS_INVALID )
                    {
                        inputMouseState.lastCursorPos = cursorPos;
                    }

                    auto & eInputMouseMove = pOutEvent.eInputMouseMove;
                    eInputMouseMove.eventCode = E_EVENT_CODE_INPUT_MOUSE_MOVE;
                    eInputMouseMove.cursorPos = cursorPos;

                    // Old method, based on the state mask provided by the system. Although *some* modifiers are there,
                    // extra mouse buttons (X1 and X2) are not reported. Since there is our own internal state mask kept
                    // inside the EventInputState object, we just use that. This line is supposed to be kept here just
                    // for the future reference or if additional modifiers need to be supported.
                    // eInputMouseMove.buttonStateMask = _x11GetMouseButtonStateMask( pXEvent.xmotion.state );

                    eInputMouseMove.buttonStateMask = inputMouseState.buttonStateMask;
                    eInputMouseMove.movementDelta = cursorPos - inputMouseState.lastCursorPos;

                    inputMouseState.lastCursorPos = cursorPos;

                    break;
                }
                default:
                {
                    return false;
                }
            }

            return pOutEvent.commonData.eventCode != E_EVENT_CODE_UNDEFINED;
        }

        bool _x11TranslateInputEventMouseButton( X11EventController & pEventController, const XEvent & pXEvent, EventObject & pOutEvent, EMouseButtonActionType pButtonAction )
        {
            auto & inputMouseState = pEventController.getEventDispatcherInputState().inputMouseState;

            const math::Vec2i32 cursorPos { pXEvent.xbutton.x, pXEvent.xbutton.y };
            const auto EX11MouseButtonID = _x11GetMouseButtonID( pXEvent.xbutton.button );

            switch( EX11MouseButtonID )
            {
                case EX11MouseButtonID::Left:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = pButtonAction;
                    eInputMouseButton.buttonID = EMouseButtonID::Left;

                    if( pButtonAction == EMouseButtonActionType::Click )
                    {
                        inputMouseState.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_LEFT_BIT );
                    }
                    else if( pButtonAction == EMouseButtonActionType::Release )
                    {
                        inputMouseState.buttonStateMask.unset( E_MOUSE_BUTTON_FLAG_LEFT_BIT );
                    }

                    break;
                }
                case EX11MouseButtonID::Middle:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = pButtonAction;
                    eInputMouseButton.buttonID = EMouseButtonID::Middle;

                    if( pButtonAction == EMouseButtonActionType::Click )
                    {
                        inputMouseState.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_MIDDLE_BIT );
                    }
                    else if( pButtonAction == EMouseButtonActionType::Release )
                    {
                        inputMouseState.buttonStateMask.unset( E_MOUSE_BUTTON_FLAG_MIDDLE_BIT );
                    }

                    break;
                }
                case EX11MouseButtonID::Right:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = pButtonAction;
                    eInputMouseButton.buttonID = EMouseButtonID::Right;

                    if( pButtonAction == EMouseButtonActionType::Click )
                    {
                        inputMouseState.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_RIGHT_BIT );
                    }
                    else if( pButtonAction == EMouseButtonActionType::Release )
                    {
                        inputMouseState.buttonStateMask.unset( E_MOUSE_BUTTON_FLAG_RIGHT_BIT );
                    }

                    break;
                }
                case EX11MouseButtonID::VWheelUp:
                {
                    auto & eInputMouseScroll = pOutEvent.eInputMouseScroll;
                    eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
                    eInputMouseScroll.scrollDelta.x = 0;
                    eInputMouseScroll.scrollDelta.y = 100;
                    break;
                }
                case EX11MouseButtonID::VWheelDown:
                {
                    auto & eInputMouseScroll = pOutEvent.eInputMouseScroll;
                    eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
                    eInputMouseScroll.scrollDelta.x = 0;
                    eInputMouseScroll.scrollDelta.y = -100;

                    break;
                }
                case EX11MouseButtonID::HWheelLeft:
                {
                    auto & eInputMouseScroll = pOutEvent.eInputMouseScroll;
                    eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
                    eInputMouseScroll.scrollDelta.x = -100;
                    eInputMouseScroll.scrollDelta.y = 0;

                    break;
                }
                case EX11MouseButtonID::HWheelRight:
                {
                    auto & eInputMouseScroll = pOutEvent.eInputMouseScroll;
                    eInputMouseScroll.eventCode = E_EVENT_CODE_INPUT_MOUSE_SCROLL;
                    eInputMouseScroll.scrollDelta.x = 100;
                    eInputMouseScroll.scrollDelta.y = 0;

                    break;
                }
                case EX11MouseButtonID::XBT1:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = pButtonAction;
                    eInputMouseButton.buttonID = EMouseButtonID::XB1;

                    if( pButtonAction == EMouseButtonActionType::Click )
                    {
                        inputMouseState.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_XB1_BIT );
                    }
                    else if( pButtonAction == EMouseButtonActionType::Release )
                    {
                        inputMouseState.buttonStateMask.unset( E_MOUSE_BUTTON_FLAG_XB1_BIT );
                    }

                    break;
                }
                case EX11MouseButtonID::XBT2:
                {
                    auto & eInputMouseButton = pOutEvent.eInputMouseButton;
                    eInputMouseButton.eventCode = E_EVENT_CODE_INPUT_MOUSE_BUTTON;
                    eInputMouseButton.cursorPos = cursorPos;
                    eInputMouseButton.buttonAction = pButtonAction;
                    eInputMouseButton.buttonID = EMouseButtonID::XB2;

                    if( pButtonAction == EMouseButtonActionType::Click )
                    {
                        inputMouseState.buttonStateMask.set( E_MOUSE_BUTTON_FLAG_XB2_BIT );
                    }
                    else if( pButtonAction == EMouseButtonActionType::Release )
                    {
                        inputMouseState.buttonStateMask.unset( E_MOUSE_BUTTON_FLAG_XB2_BIT );
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

        bool _x11TranslateSystemEvent( X11EventController & pEventController, const XEvent & pXEvent, EventObject & pOutEvent )
        {
            switch( pXEvent.type )
            {
                case ClientMessage:
                {
                    auto & xSessionData = platform::x11GetXSessionData( *( pEventController.mSysContext ) );

                    // Type of wm protocol message is stored in data.l[0].
                    long wmpMessageType = pXEvent.xclient.data.l[0];
                    if ( wmpMessageType == xSessionData.wmpDeleteWindow )
                    {
                        auto * eventSource = pEventController.findEventSource( [&pXEvent]( const EventSource & pEventSource ) -> bool {
                            const auto * eventSourceNativeData = pEventSource.getEventSourceNativeDataAs<platform::X11EventSourceNativeData>();
                            return eventSourceNativeData->windowXID == pXEvent.xclient.window;
                        });
                        auto & eventData = pOutEvent.eWindowUpdateClose;
                        eventData.eventCode = E_EVENT_CODE_WINDOW_UPDATE_CLOSE;
                        eventData.eventSource = eventSource;
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


        static const EKeyCode x11KeycodeMap_20_7A[] =
        {
            /* 0x0020 */ EKeyCode::Space,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
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
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x0041 */ EKeyCode::CharA,
            /* 0x0042 */ EKeyCode::CharB,
            /* 0x0043 */ EKeyCode::CharC,
            /* 0x0044 */ EKeyCode::CharD,
            /* 0x0045 */ EKeyCode::CharE,
            /* 0x0046 */ EKeyCode::CharF,
            /* 0x0047 */ EKeyCode::CharG,
            /* 0x0048 */ EKeyCode::CharH,
            /* 0x0049 */ EKeyCode::CharI,
            /* 0x004a */ EKeyCode::CharJ,
            /* 0x004b */ EKeyCode::CharK,
            /* 0x004c */ EKeyCode::CharL,
            /* 0x004d */ EKeyCode::CharM,
            /* 0x004e */ EKeyCode::CharN,
            /* 0x004f */ EKeyCode::CharO,
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
            /* 0x005a */ EKeyCode::CharZ,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x00xx */ EKeyCode::Unknown,
            /* 0x0061 */ EKeyCode::CharA,
            /* 0x0062 */ EKeyCode::CharB,
            /* 0x0063 */ EKeyCode::CharC,
            /* 0x0064 */ EKeyCode::CharD,
            /* 0x0065 */ EKeyCode::CharE,
            /* 0x0066 */ EKeyCode::CharF,
            /* 0x0067 */ EKeyCode::CharG,
            /* 0x0068 */ EKeyCode::CharH,
            /* 0x0069 */ EKeyCode::CharI,
            /* 0x006a */ EKeyCode::CharJ,
            /* 0x006b */ EKeyCode::CharK,
            /* 0x006c */ EKeyCode::CharL,
            /* 0x006d */ EKeyCode::CharM,
            /* 0x006e */ EKeyCode::CharN,
            /* 0x006f */ EKeyCode::CharO,
            /* 0x0070 */ EKeyCode::CharP,
            /* 0x0071 */ EKeyCode::CharQ,
            /* 0x0072 */ EKeyCode::CharR,
            /* 0x0073 */ EKeyCode::CharS,
            /* 0x0074 */ EKeyCode::CharT,
            /* 0x0075 */ EKeyCode::CharU,
            /* 0x0076 */ EKeyCode::CharV,
            /* 0x0077 */ EKeyCode::CharW,
            /* 0x0078 */ EKeyCode::CharX,
            /* 0x0079 */ EKeyCode::CharY,
            /* 0x007a */ EKeyCode::CharZ,
        };

        static const EKeyCode x11KeycodeMap_50_57[] =
        {
            /* 0xff50 */ EKeyCode::Home,
            /* 0xff51 */ EKeyCode::ArrowLeft,
            /* 0xff52 */ EKeyCode::ArrowUp,
            /* 0xff53 */ EKeyCode::ArrowRight,
            /* 0xff54 */ EKeyCode::ArrowDown,
            /* 0xff55 */ EKeyCode::PageUp,
            /* 0xff56 */ EKeyCode::PageDown,
            /* 0xff57 */ EKeyCode::End
        };

        static const EKeyCode x11KeycodeMap_AA_B9[] =
        {
            /* 0xffaa */ EKeyCode::NumPadMul,
            /* 0xffab */ EKeyCode::NumPadAdd,
            /* 0xffac */ EKeyCode::Unknown,
            /* 0xffad */ EKeyCode::NumPadSub,
            /* 0xffae */ EKeyCode::NumPadDot,
            /* 0xffaf */ EKeyCode::NumPadDiv,
            /* 0xffb0 */ EKeyCode::NumPad0,
            /* 0xffb1 */ EKeyCode::NumPad1,
            /* 0xffb2 */ EKeyCode::NumPad2,
            /* 0xffb3 */ EKeyCode::NumPad3,
            /* 0xffb4 */ EKeyCode::NumPad4,
            /* 0xffb5 */ EKeyCode::NumPad5,
            /* 0xffb6 */ EKeyCode::NumPad6,
            /* 0xffb7 */ EKeyCode::NumPad7,
            /* 0xffb8 */ EKeyCode::NumPad8,
            /* 0xffb9 */ EKeyCode::NumPad9,
        };

        static const EKeyCode x11KeycodeMap_BE_C9[] =
        {
            /* 0xffbe */ EKeyCode::F1,
            /* 0xffbf */ EKeyCode::F2,
            /* 0xffc0 */ EKeyCode::F3,
            /* 0xffc1 */ EKeyCode::F4,
            /* 0xffc2 */ EKeyCode::F5,
            /* 0xffc3 */ EKeyCode::F6,
            /* 0xffc4 */ EKeyCode::F7,
            /* 0xffc5 */ EKeyCode::F8,
            /* 0xffc6 */ EKeyCode::F9,
            /* 0xffc7 */ EKeyCode::F10,
            /* 0xffc8 */ EKeyCode::F11,
            /* 0xffc9 */ EKeyCode::F12
        };

        static const EKeyCode x11KeycodeMap_E1_EA[] =
        {
            /* 0xffe1 */ EKeyCode::ShiftLeft,
            /* 0xffe2 */ EKeyCode::ShiftRight,
            /* 0xffe3 */ EKeyCode::CtrlLeft,
            /* 0xffe4 */ EKeyCode::CtrlRight,
            /* 0xffe5 */ EKeyCode::CapsLock,
            /* 0xffe6 */ EKeyCode::Unknown,
            /* 0xffe7 */ EKeyCode::Unknown,
            /* 0xffe8 */ EKeyCode::Unknown,
            /* 0xffe9 */ EKeyCode::AltLeft,
            /* 0xffea */ EKeyCode::AltRight
        };


        EKeyCode _x11GetSysKeyCode( KeySym pXkeySym )
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
                return EKeyCode::Tab;
            }
            else if ( pXkeySym == XK_Return )
            {
                return EKeyCode::Enter;
            }
            else if ( pXkeySym == XK_Escape )
            {
                return EKeyCode::Escape;
            }
            else if ( pXkeySym == XK_BackSpace )
            {
                return EKeyCode::Backspace;
            }
            else if ( pXkeySym == XK_Insert )
            {
                return EKeyCode::Insert;
            }
            else if ( pXkeySym == XK_Delete )
            {
                return EKeyCode::Delete;
            }

            return EKeyCode::Unknown;
        }

        static constexpr EX11MouseButtonID x11MouseButtonIDMap[] =
        {
            EX11MouseButtonID::Left,
            EX11MouseButtonID::Middle,
            EX11MouseButtonID::Right,
            EX11MouseButtonID::VWheelUp,
            EX11MouseButtonID::VWheelDown,
            EX11MouseButtonID::HWheelLeft,
            EX11MouseButtonID::HWheelRight,
            EX11MouseButtonID::XBT1,
            EX11MouseButtonID::XBT2,
            EX11MouseButtonID::Unknown
        };
        static constexpr auto x11MouseButtonMaxIndex = staticArraySize( x11MouseButtonIDMap ) - 1;

        EX11MouseButtonID _x11GetMouseButtonID( uint32 pButton )
        {
            auto mouseButtonIndex = static_cast<size_t>( pButton - 1 );
            mouseButtonIndex = getMinOf( mouseButtonIndex, x11MouseButtonMaxIndex );
            return x11MouseButtonIDMap[mouseButtonIndex];
        }

        Bitmask<EMouseButtonFlagBits> _x11GetMouseButtonStateMask( uint32 pState )
        {
            Bitmask<uint32> inputStateMask = pState;
            Bitmask<EMouseButtonFlagBits> buttonStateMask;
            if( pState != 0 )
            {
                if( inputStateMask.isSetAnyOf( Button1Mask | Button1MotionMask ) )
                {
                    inputStateMask.unset( Button1Mask | Button1MotionMask );
                    buttonStateMask.set( E_MOUSE_BUTTON_FLAG_LEFT_BIT );
                }
                if( inputStateMask.isSetAnyOf( Button2Mask | Button2MotionMask ) )
                {
                    inputStateMask.unset( Button2Mask | Button2MotionMask );
                    buttonStateMask.set( E_MOUSE_BUTTON_FLAG_MIDDLE_BIT );
                }
                if( inputStateMask.isSetAnyOf( Button3Mask | Button3MotionMask ) )
                {
                    inputStateMask.unset( Button3Mask | Button3MotionMask );
                    buttonStateMask.set( E_MOUSE_BUTTON_FLAG_RIGHT_BIT);
                }
            }
            return buttonStateMask;
        }
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
