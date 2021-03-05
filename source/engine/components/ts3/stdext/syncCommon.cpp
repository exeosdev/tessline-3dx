
#include "syncCommon.h"
#include <emmintrin.h>
#include <thread>

namespace ts3
{

	void syncHardwarePause()
	{
		_mm_pause();
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
			_mm_pause();
		}
		else
		{
			std::this_thread::yield();
		}
	}

}
