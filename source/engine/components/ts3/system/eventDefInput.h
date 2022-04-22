
#ifndef __TS3_SYSTEM_EVENT_DEF_INPUT_H__
#define __TS3_SYSTEM_EVENT_DEF_INPUT_H__

#include "eventCommon.h"

namespace ts3::system
{

	struct EvtInput : public EvtBase
	{
	};

	struct EvtInputGamepadAxis : public EvtInput
	{
	};

	struct EvtInputGamepadButton : public EvtInput
	{
	};

	struct EvtInputGamepadState : public EvtInput
	{
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_INPUT_H__
