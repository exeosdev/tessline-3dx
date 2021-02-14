
#include "posixPerfCounter.h"

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_POSIX )
namespace ts3
{

	sys_perf_counter_value_t nativePerfCounterQueryValue()
	{
		timespec timeSpec {};
		clock_gettime( CLOCK_MONOTONIC, &timeSpec );
		return static_cast<uint64>( timeSpec.tv_sec ) * 1000000000u + static_cast<uint64>( timeSpec.tv_nsec );
	}

	sys_perf_counter_res_t nativePerfCounterQueryResolution()
	{
		return 1000000000u;
	}

} // namespace ts3
#endif
