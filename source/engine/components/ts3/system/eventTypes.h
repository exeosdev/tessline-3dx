
#ifndef __TS3_SYSTEM_EVENT_TYPES_DEF_H__
#define __TS3_SYSTEM_EVENT_TYPES_DEF_H__

#include "eventDefs.h"
#include <ts3/math/vector.h>

namespace ts3
{

	template <sys_event_code_value_t tpEventCode>
	struct SysEvtApp : public SysEvtBase
	{
	public:
		SysEvtApp()
			: SysEvtBase( tpEventCode )
		{}

		static_assert( ExfEnumGetEventCodeBaseType( tpEventCode ) == ESysEventBaseType::App );
	};

	struct SysEvtAppActivityDisplayReady : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_DISPLAY_READY>
	{
	};

	struct SysEvtAppActivityFocusGained : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED>
	{
	};

	struct SysEvtAppActivityFocusLost : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST>
	{
	};

	struct SysEvtAppActivityPause : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_PAUSE>
	{
	};

	struct SysEvtAppActivityResume : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_RESUME>
	{
	};

	struct SysEvtAppActivityStart : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_START>
	{
	};

	struct SysEvtAppActivityStop : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_STOP>
	{
	};

	struct SysEvtAppActivityQuit : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_QUIT>
	{
	};

	struct SysEvtAppActivityTerminate : public SysEvtApp<E_SYS_EVENT_CODE_APP_ACTIVITY_TERMINATE>
	{
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class SysKeyActionType : enum_default_value_t
	{
		Press,
		Release
	};

	enum class SysKeyCode : uint32
	{
		Unknown = cxUint32Max,
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

	inline constexpr auto cvEnumSysKeyCodeCount = static_cast< size_t >( SysKeyCode::_Reserved );

	struct SysKeyboardState
	{
	};

	enum SysMouseButtonFlagBits : uint32
	{
		SYS_MOUSE_BUTTON_FLAG_LEFT_BIT = 0x0001,
		SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT = 0x0002,
		SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT = 0x0010,
		SYS_MOUSE_BUTTON_FLAG_XB1_BIT = 0x0020,
		SYS_MOUSE_BUTTON_FLAG_XB2_BIT = 0x0040,
		SYS_MOUSE_BUTTON_FLAG_ALL_BIT = 0x00FF
	};

	enum class SysMouseButtonID : enum_default_value_t
	{
		Unknown,
		Left = static_cast<enum_default_value_t>( SYS_MOUSE_BUTTON_FLAG_LEFT_BIT ),
		Right = static_cast<enum_default_value_t>( SYS_MOUSE_BUTTON_FLAG_RIGHT_BIT ),
		Middle = static_cast<enum_default_value_t>( SYS_MOUSE_BUTTON_FLAG_MIDDLE_BIT ),
		XB1 = static_cast<enum_default_value_t>( SYS_MOUSE_BUTTON_FLAG_XB1_BIT ),
		XB2 = static_cast<enum_default_value_t>( SYS_MOUSE_BUTTON_FLAG_XB2_BIT ),
	};

	enum class SysMouseButtonActionType : enum_default_value_t
	{
		Click,
		DoubleClick,
		MultiClick,
		Release
	};

	template <sys_event_code_value_t tpEventCode>
	struct SysEvtInput : public SysEvtBase
	{
	public:
		SysEvtInput()
			: SysEvtBase( tpEventCode )
		{}

		static_assert( ExfEnumGetEventCodeBaseType( tpEventCode ) == ESysEventBaseType::Input );
	};

	struct SysEvtInputGamepadAxis : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_GAMEPAD_AXIS>
	{
	};


	struct SysEvtInputGamepadButton : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_GAMEPAD_BUTTON>
	{
	};

	struct SysEvtInputGamepadState : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_GAMEPAD_STATE>
	{
	};

	struct SysEvtInputKeyboardKey : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_KEYBOARD_KEY>
	{
		//
		SysKeyActionType keyAction;
		//
		SysKeyCode keyCode;
		//
		const SysKeyboardState * keyboardState;
	};

	struct SysEvtInputMouseButton : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON>
	{
		//
		SysMouseButtonActionType buttonAction;
		//
		SysMouseButtonID buttonID;
		//
		Bitmask<SysMouseButtonFlagBits> buttonStateMask;
		//
		uint32 multiClickSequenceLength;
		//
		math::Vec2i32 cursorPos;
	};

	struct SysEvtInputMouseMove : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_MOUSE_MOVE>
	{
		//
		Bitmask<SysMouseButtonFlagBits> buttonStateMask;
		//
		math::Vec2i32 cursorPos;
		//
		math::Vec2i32 movementDelta;
	};

	struct SysEvtInputMouseScroll : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL>
	{
		//
		math::Vec2i32 scrollDelta;
	};

	struct SysEvtInputTouchDown : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_TOUCH_DOWN>
	{
	};

	struct SysEvtInputTouchMove : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_TOUCH_MOVE>
	{
	};

	struct SysEvtInputTouchUp : public SysEvtInput<E_SYS_EVENT_CODE_INPUT_TOUCH_UP>
	{
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <sys_event_code_value_t tpEventCode>
	struct SysEvtWindow : public SysEvtBase
	{
	public:
		SysEvtWindow()
			: SysEvtBase( tpEventCode )
		{}

		static_assert( ExfEnumGetEventCodeBaseType( tpEventCode ) == ESysEventBaseType::Window );
	};

	struct SysEvtWindowUpdateClose : public SysEvtWindow<E_SYS_EVENT_CODE_WINDOW_UPDATE_CLOSE>
	{
	};

	struct SysEvtWindowUpdateFullscreen : public SysEvtWindow<E_SYS_EVENT_CODE_WINDOW_UPDATE_FULLSCREEN>
	{
	};

	struct SysEvtWindowUpdateResize : public SysEvtWindow<E_SYS_EVENT_CODE_WINDOW_UPDATE_RESIZE>
	{
	};

	struct SysEvtWindowUpdateVisibility : public SysEvtWindow<E_SYS_EVENT_CODE_WINDOW_UPDATE_VISIBILITY>
	{
	};

}

#endif // __TS3_SYSTEM_EVENT_TYPES_DEF_H__
