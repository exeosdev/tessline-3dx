
#ifndef __TS3_SYSTEM_EVENT_CORE_TYPES_H__
#define __TS3_SYSTEM_EVENT_CORE_TYPES_H__

#include "eventCommon.h"
#include "windowCommon.h"
#include <ts3/math/vector.h>

namespace ts3::system
{

	template <event_code_value_t tpEventCode>
	struct EvtApp : public EvtBase
	{
	public:
	    constexpr EvtApp()
        : EvtBase( tpEventCode )
		{}
	};

	struct EvtAppActivityDisplayReady : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_DISPLAY_READY>
	{
	};

	struct EvtAppActivityFocusGained : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED>
	{
	};

	struct EvtAppActivityFocusLost : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST>
	{
	};

	struct EvtAppActivityPause : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_PAUSE>
	{
	};

	struct EvtAppActivityResume : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_RESUME>
	{
	};

	struct EvtAppActivityStart : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_START>
	{
	};

	struct EvtAppActivityStop : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_STOP>
	{
	};

	struct EvtAppActivityQuit : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_QUIT>
	{
	};

	struct EvtAppActivityTerminate : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_TERMINATE>
	{
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class EKeyActionType : enum_default_value_t
	{
		Press,
		Release
	};

	enum class EKeyCode : uint32
	{
		Unknown = CX_UINT32_MAX,
		Num0 = 0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		Numpad0,
		Numpad1,
		Numpad2,
		Numpad3,
		Numpad4,
		Numpad5,
		Numpad6,
		Numpad7,
		Numpad8,
		Numpad9,
		NumpadAdd,
		NumpadSub,
		NumpadMul,
		NumpadDiv,
		NumpadDot,
		ArrowLeft,
		ArrowUp,
		ArrowRight,
		ArrowDown,
		PageUp,
		PageDown,
		CtrlLeft,
		CtrlRight,
		ShiftLeft,
		ShiftRight,
		AltLeft,
		AltRight,
		CapsLock,
		Enter,
		Backspace,
		Tab,
		Escape,
		Space,
		End,
		Home,
		Insert,
		Delete,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		CharA = 65,
		CharB,
		CharC,
		CharD,
		CharE,
		CharF,
		CharG,
		CharH,
		CharI,
		CharJ,
		CharK,
		CharL,
		CharM,
		CharN,
		CharO,
		CharP,
		CharQ,
		CharR,
		CharS,
		CharT,
		CharU,
		CharV,
		CharW,
		CharX,
		CharY,
		CharZ,
		_Reserved
	};

	inline constexpr auto cvEnumKeyCodeCount = static_cast< size_t >( EKeyCode::_Reserved );

	struct KeyboardState
	{
	};

	enum EMouseButtonFlagBits : uint32
	{
		E_SYS_MOUSE_BUTTON_FLAG_LEFT_BIT = 0x0001,
		E_SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT = 0x0002,
		E_SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT = 0x0010,
		E_SYS_MOUSE_BUTTON_FLAG_XB1_BIT = 0x0020,
		E_SYS_MOUSE_BUTTON_FLAG_XB2_BIT = 0x0040,
		E_SYS_MOUSE_BUTTON_FLAG_ALL_BIT = 0x00FF
	};

	enum class EMouseButtonID : enum_default_value_t
	{
		Unknown,
		Left = static_cast<enum_default_value_t>( E_SYS_MOUSE_BUTTON_FLAG_LEFT_BIT ),
		Right = static_cast<enum_default_value_t>( E_SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT ),
		Middle = static_cast<enum_default_value_t>( E_SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT ),
		XB1 = static_cast<enum_default_value_t>( E_SYS_MOUSE_BUTTON_FLAG_XB1_BIT ),
		XB2 = static_cast<enum_default_value_t>( E_SYS_MOUSE_BUTTON_FLAG_XB2_BIT ),
	};

	enum class EMouseButtonActionType : enum_default_value_t
	{
		Click,
		DoubleClick,
		MultiClick,
		Release
	};

	template <event_code_value_t tpEventCode>
	struct EvtInput : public EvtBase
	{
	public:
	    constexpr EvtInput()
        : EvtBase( tpEventCode )
		{}
	};

	struct EvtInputGamepadAxis : public EvtInput<E_EVENT_CODE_INPUT_GAMEPAD_AXIS>
	{
	};


	struct EvtInputGamepadButton : public EvtInput<E_EVENT_CODE_INPUT_GAMEPAD_BUTTON>
	{
	};

	struct EvtInputGamepadState : public EvtInput<E_EVENT_CODE_INPUT_GAMEPAD_STATE>
	{
	};

	struct EvtInputKeyboardKey : public EvtInput<E_EVENT_CODE_INPUT_KEYBOARD_KEY>
	{
		//
		EKeyActionType keyAction;
		//
		EKeyCode keyCode;
		//
		const KeyboardState * keyboardState;
	};

	struct EvtInputMouseButton : public EvtInput<E_EVENT_CODE_INPUT_MOUSE_BUTTON>
	{
		//
		EMouseButtonActionType buttonAction;
		//
		EMouseButtonID buttonID;
		//
		Bitmask<EMouseButtonFlagBits> buttonStateMask;
		//
		uint32 multiClickSequenceLength;
		//
		math::Vec2i32 cursorPos;
	};

	struct EvtInputMouseMove : public EvtInput<E_EVENT_CODE_INPUT_MOUSE_MOVE>
	{
		//
		Bitmask<EMouseButtonFlagBits> buttonStateMask;
		//
		math::Vec2i32 cursorPos;
		//
		math::Vec2i32 movementDelta;
	};

	struct EvtInputMouseScroll : public EvtInput<E_EVENT_CODE_INPUT_MOUSE_SCROLL>
	{
		//
		math::Vec2i32 scrollDelta;
	};

	struct EvtInputTouchDown : public EvtInput<E_EVENT_CODE_INPUT_TOUCH_DOWN>
	{
	};

	struct EvtInputTouchMove : public EvtInput<E_EVENT_CODE_INPUT_TOUCH_MOVE>
	{
	};

	struct EvtInputTouchUp : public EvtInput<E_EVENT_CODE_INPUT_TOUCH_UP>
	{
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Window;

	template <event_code_value_t tpEventCode>
	struct EvtWindow : public EvtBase
	{
	public:
	    Window * sourceWindow = nullptr;

	public:
	    constexpr EvtWindow()
        : EvtBase( tpEventCode )
		{}
	};

	struct EvtWindowUpdateClose : public EvtWindow<E_EVENT_CODE_WINDOW_UPDATE_CLOSE>
	{
	};

	struct EvtWindowUpdateFullscreen : public EvtWindow<E_EVENT_CODE_WINDOW_UPDATE_FULLSCREEN>
	{
	};

	struct EvtWindowUpdateResize : public EvtWindow<E_EVENT_CODE_WINDOW_UPDATE_RESIZE>
	{
	    WindowSize newSize;
	};

	struct EvtWindowUpdateVisibility : public EvtWindow<E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY>
	{
	    bool newVisibilityState;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_CORE_TYPES_H__
