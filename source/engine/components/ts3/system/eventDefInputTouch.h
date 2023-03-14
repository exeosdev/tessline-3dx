
#ifndef __TS3_SYSTEM_EVENT_DEF_INPUT_TOUCH_H__
#define __TS3_SYSTEM_EVENT_DEF_INPUT_TOUCH_H__

#include "eventDefInput.h"

namespace ts3::system
{

	struct EvtInputTouchDown : public EvtInput
	{
	};

	struct EvtInputTouchMove : public EvtInput
	{
	};

	struct EvtInputTouchUp : public EvtInput
	{
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_INPUT_TOUCH_H__
