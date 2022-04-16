
#ifndef __TS3_SYSTEM_PERF_COUNTER_H__
#define __TS3_SYSTEM_PERF_COUNTER_H__

#include "prerequisites.h"
#include <ts3/stdext/chrono.h>

namespace ts3::system
{

	/// @brief Type used to represent the resolution of a PerfCounter.
	using perf_counter_res_t = uint64;

	/// @brief Type used to represent values of stamps generated by a PerfCounter.
	using perf_counter_value_t = uint64;

	/// @brief
	using perf_counter_ratio_t = std::pair<native_int, native_int>;

	class PerfCounter
	{
	public:
	/// @brief Queries and returns the current value of a system-specific, high-performance clock.
		/// Returned value should not be analysed or interpreted directly by the app - its unit is
		/// platform-specific and my yield number of ticks, nanoseconds or some other type of duration.
		TS3_SYSTEM_API_NODISCARD static perf_counter_value_t queryCurrentStamp();

		/// @brief Returns the resolution of a counter as a number of units which elapse during a period of one second.
		TS3_SYSTEM_API_NODISCARD static perf_counter_res_t queryResolution();

		/// @brief Converts a difference between two PC stamps to a duration value using specified unit ratio.
		/// Ratio represents a unit expressed as a fraction relative to a second. I.e. converting to nanoseconds
		/// would be done by calling: convertToDuration( timeStampDiff, perf_counter_ratio_t{ 1, 1000000000 } );
		TS3_SYSTEM_API_NODISCARD static long double convertToDuration( perf_counter_value_t pStampDiff,
		                                                               const perf_counter_ratio_t & pUnitRatio );

		/// @brief Converts a duration value back to a PC time stamp difference using specified unit ratio.
		/// Assertion: convertFromDuration( convertToDuration( timeStampDiff, R ), R ) == timeStampDiff for any given R.
		TS3_SYSTEM_API_NODISCARD static perf_counter_value_t convertFromDuration( long double pDuration,
		                                                                          const perf_counter_ratio_t & pUnitRatio );

		/// @brief Helper function which converts a perf counter duration to a duration value in unit expressed as ts3::EDurationPeriod.
		template <EDurationPeriod tpPeriod>
		TS3_SYSTEM_API_NODISCARD static long double convertToDuration( perf_counter_value_t pStampDiff )
		{
			return convertToDuration( pStampDiff, DurationTraits<tpPeriod>::unitRatio );
		}

		/// @brief Helper function which converts a duration value in unit expressed as ts3::EDurationPeriod back to a PC time stamp difference.
		template <EDurationPeriod tpPeriod>
		TS3_SYSTEM_API_NODISCARD static perf_counter_value_t convertFromDuration( const Duration<tpPeriod> & pDuration )
		{
			return convertFromDuration( pDuration.count(), DurationTraits<tpPeriod>::unitRatio );
		}
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PERF_COUNTER_H__
