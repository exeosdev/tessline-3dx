
#ifndef __TS3_SYSTEM_EVENT_DEF_INPUT_H__
#define __TS3_SYSTEM_EVENT_DEF_INPUT_H__

#include "eventCommon.h"

namespace ts3::system
{

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

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_INPUT_H__
