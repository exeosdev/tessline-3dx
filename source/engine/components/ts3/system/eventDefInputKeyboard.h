
#ifndef __TS3_SYSTEM_EVENT_DEF_INPUT_KEYBOARD_H__
#define __TS3_SYSTEM_EVENT_DEF_INPUT_KEYBOARD_H__

#include "eventDefInput.h"

namespace ts3::system
{

	enum class EKeyActionType : enum_default_value_t;
	enum class EKeyCode : uint32;

	enum class EKeyActionType : enum_default_value_t
	{
		Press,
		Release
	};

	enum class EKeyCode : uint32
	{
		Unknown = CX_UINT32_MAX,

		Num0 = 0, // 0x00
		Num1 = 1, // 0x01
		Num2 = 2, // 0x02
		Num3 = 3, // 0x03
		Num4 = 4, // 0x04
		Num5 = 5, // 0x05
		Num6 = 6, // 0x06
		Num7 = 7, // 0x07
		Num8 = 8, // 0x08
		Num9 = 9, // 0x09

		NumPad0	= 10, // 0x0A
		NumPad1	= 11, // 0x0B
		NumPad2	= 12, // 0x0C
		NumPad3	= 13, // 0x0D
		NumPad4	= 14, // 0x0E
		NumPad5	= 15, // 0x0F
		NumPad6	= 16, // 0x10
		NumPad7	= 17, // 0x11
		NumPad8	= 18, // 0x12
		NumPad9	= 19, // 0x13
		NumPadAdd  = 20, // 0x14
		NumPadSub  = 21, // 0x15
		NumPadMul  = 22, // 0x16
		NumPadDiv  = 23, // 0x17
		NumPadDot  = 24, // 0x18

		Escape	   = 27, // 0x1B
		ArrowLeft  = 28, // 0x1C
		ArrowUp	   = 29, // 0x1D
		ArrowRight = 30, // 0x1E
		ArrowDown  = 31, // 0x1F
		Space	   = 32, // 0x20
		Enter	   = 33, // 0x21
		Backspace  = 34, // 0x22
		Tab		   = 35, // 0x23
		End		   = 36, // 0x24
		Home	   = 37, // 0x25
		Insert	   = 38, // 0x26
		Delete	   = 39, // 0x27
		CapsLock   = 40, // 0x28
		PageUp	   = 41, // 0x29
		PageDown   = 42, // 0x2A
		CtrlLeft   = 43, // 0x2B
		CtrlRight  = 44, // 0x2C
		ShiftLeft  = 45, // 0x2D
		ShiftRight = 46, // 0x2E
		AltLeft	   = 47, // 0x2F
		AltRight   = 48, // 0x30

		F1  = 49, // 0x31
		F2  = 50, // 0x32
		F3  = 51, // 0x33
		F4  = 52, // 0x34
		F5  = 53, // 0x35
		F6  = 54, // 0x36
		F7  = 55, // 0x37
		F8  = 56, // 0x38
		F9  = 57, // 0x39
		F10 = 58, // 0x3A
		F11 = 59, // 0x3B
		F12 = 60, // 0x3C

		CharA = 65, // 0x41
		CharB = 66, // 0x42
		CharC = 67, // 0x43
		CharD = 68, // 0x44
		CharE = 69, // 0x45
		CharF = 70, // 0x46
		CharG = 71, // 0x47
		CharH = 72, // 0x48
		CharI = 73, // 0x49
		CharJ = 74, // 0x4A
		CharK = 75, // 0x4B
		CharL = 76, // 0x4C
		CharM = 77, // 0x4D
		CharN = 78, // 0x4E
		CharO = 79, // 0x4F
		CharP = 80, // 0x50
		CharQ = 81, // 0x51
		CharR = 82, // 0x52
		CharS = 83, // 0x53
		CharT = 84, // 0x54
		CharU = 85, // 0x55
		CharV = 86, // 0x56
		CharW = 87, // 0x57
		CharX = 88, // 0x58
		CharY = 89, // 0x59
		CharZ = 90, // 0x5A

		_ReservedMax,
	};

	inline constexpr auto CX_ENUM_KEY_CODE_COUNT = static_cast< size_t >( EKeyCode::_ReservedMax );

	struct KeyStateMap
	{
		bool stateArray[CX_ENUM_KEY_CODE_COUNT];

		bool & operator[]( size_t pIndex )
		{
			return staticArrayElement( stateArray, pIndex );
		}

		bool operator[]( size_t pIndex ) const
		{
			return staticArrayElement( stateArray, pIndex );
		}

		bool & operator[]( EKeyCode pKeyCode )
		{
			return staticArrayElement( stateArray, pKeyCode );
		}

		bool operator[]( EKeyCode pKeyCode ) const
		{
			return staticArrayElement( stateArray, pKeyCode );
		}
	};

	struct EvtSharedInputKeyboardState
	{
		EventDispatcher *
		KeyStateMap keyStateMap;
	};

	struct EvtInputKeyboard : public EvtInput
	{
		//
		EKeyActionType keyAction;
		//
		EKeyCode keyCode;
		//
		const EvtSharedInputKeyboardState * inputKeyboardState;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_INPUT_KEYBOARD_H__
