
#pragma once

#ifndef __TS3_CORE_SIGNAL_COMMON_H__
#define __TS3_CORE_SIGNAL_COMMON_H__

#include "../prerequisites.h"
#include <functional>

namespace ts3
{

	using signal_code_value_t = uint32;

	template <signal_code_value_t tpSignalCode, typename... TpSignalArgs>
	struct Signal
	{
		static constexpr signal_code_value_t signalCode = tpSignalCode;
	};

}

#endif // __TS3_CORE_SIGNAL_COMMON_H__
