
#ifndef __TS3_SYSTEM_EVENT_DEF_INPUT_TOUCH_H__
#define __TS3_SYSTEM_EVENT_DEF_INPUT_TOUCH_H__

#include "eventDefInput.h"

namespace ts3::system
{

	struct EvtInputTouchDown : public EvtInput<E_EVENT_CODE_INPUT_TOUCH_DOWN>
	{
	};

	struct EvtInputTouchMove : public EvtInput<E_EVENT_CODE_INPUT_TOUCH_MOVE>
	{
	};

	struct EvtInputTouchUp : public EvtInput<E_EVENT_CODE_INPUT_TOUCH_UP>
	{
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_INPUT_TOUCH_H__
