
#ifndef __TS3_PLATFORM_SYNC_COMMON_H__
#define __TS3_PLATFORM_SYNC_COMMON_H__

#include "platform.h"

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

#endif /* __TS3_PLATFORM_SYNC_COMMON_H__ */
