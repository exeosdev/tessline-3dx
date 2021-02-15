
#pragma once

#ifndef __TS3_CORE_SIGNAL_COMMON_H__
#define __TS3_CORE_SIGNAL_COMMON_H__

#include "../prerequisites.h"
#include <functional>

namespace ts3
{

	using event_code_value_t = uint32;

	template <event_code_value_t tpEventCode, typename... TpEventArgs>
	struct Event
	{
		static constexpr event_code_value_t eventCode = tpEventCode;
	};

}

#endif // __TS3_CORE_SIGNAL_COMMON_H__
