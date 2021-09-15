
#include <ts3/system/perfCounter.h>
#include "wnfPerfCounter.h"

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_WINFAMILY )
namespace ts3::system
{

	perf_counter_value_t nativePerfCounterQueryValue()
	{
		LARGE_INTEGER perfCounter;
		QueryPerformanceCounter( &perfCounter );

		return static_cast<perf_counter_value_t>( perfCounter.QuadPart );
	}

	perf_counter_res_t nativePerfCounterQueryResolution()
	{
		// "The frequency of the performance counter is fixed at system boot and is consistent across all processors.
		// Therefore, the frequency need only be queried upon application initialization, and the result can be cached."
		// Source: https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
		static perf_counter_res_t perfCounterFrequency = 0;

		if( perfCounterFrequency == 0 )
		{
			LARGE_INTEGER counterFrequency;
			QueryPerformanceFrequency( &counterFrequency );
			perfCounterFrequency = static_cast<perf_counter_res_t>( counterFrequency.QuadPart );
		}

		return perfCounterFrequency;
	}

} // namespace ts3::system
#endif
