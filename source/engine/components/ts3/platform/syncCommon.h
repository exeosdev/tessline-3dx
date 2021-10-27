
#ifndef __TS3_STDEXT_SHARED_LOCK_H__
#define __TS3_STDEXT_SHARED_LOCK_H__

#include "prerequisites.h"
#include <atomic>
#include <shared_mutex>

namespace ts3
{

	namespace sync
	{

	    /// @brief
	    void hardwarePause();

	    /// @brief
	    void yieldCurrentThread();

	    /// @brief
	    /// @param pCounter
	    void yieldCurrentThreadAuto( uint64_t pCounter = 0 );

	}

}

#endif /* __TS3_STDEXT_SHARED_LOCK_H__ */
