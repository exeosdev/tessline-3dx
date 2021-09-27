
#include "syncCommon.h"
#include <thread>

namespace ts3
{

	void syncHardwarePause()
	{
	#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_LEVEL_SSE )
		_mm_pause();
	#endif
	}

	void syncThreadYield()
	{
		std::this_thread::yield();
	}

	void syncThreadYieldAuto( uint64_t pCounter )
	{
		constexpr uint64_t sBusySpinCount = 6;
		constexpr uint64_t sSpinCountBeforeYield = 32;

		if( pCounter < sBusySpinCount )
		{
		}
		else if( pCounter < sSpinCountBeforeYield )
		{
			syncHardwarePause();
		}
		else
		{
			std::this_thread::yield();
		}
	}

}
