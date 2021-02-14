
#ifndef __TS3_ENGINE_COMMON_GPU_DEFS_H__
#define __TS3_ENGINE_COMMON_GPU_DEFS_H__

#include "../prerequisites.h"
#include <ts3/core/signals/signalEmitter.h>

namespace ts3
{

	enum : event_code_value_t
	{
		E_EVENT_HWB_GPU_BUFFER_REF_UPDATED = 0x7F
	};

}

#endif // __TS3_ENGINE_COMMON_GPU_DEFS_H__
