
#pragma once

#include "prerequisites.h"
#include <chrono>

namespace ts3
{

	using duration_value_t = uint64;

	enum class DurationPeriod : enum_default_value_t
	{
		Nanosecond,
		Microsecond,
		Millisecond,
		Second
	};

	template <DurationPeriod>
	struct DurationTypeWrapper;

	template <>
	struct DurationTypeWrapper<DurationPeriod::Nanosecond>
	{
		using Type = std::chrono::nanoseconds;
	};

	template <>
	struct DurationTypeWrapper<DurationPeriod::Microsecond>
	{
		using Type = std::chrono::microseconds;
	};

	template <>
	struct DurationTypeWrapper<DurationPeriod::Millisecond>
	{
		using Type = std::chrono::milliseconds;
	};

	template <>
	struct DurationTypeWrapper<DurationPeriod::Second>
	{
		using Type = std::chrono::seconds;
	};


	template <DurationPeriod tpPeriod>
	using Duration = typename DurationTypeWrapper<tpPeriod>::Type;


	template <DurationPeriod tpPeriod>
	struct DurationTraits
	{
		using RatioType = typename DurationTypeWrapper<tpPeriod>::Type::period;

		static constexpr std::pair<native_int, native_int> unitRatio {
			static_cast<native_int>( RatioType::num ),
			static_cast<native_int>( RatioType::den )
		};
	};

	using Nanoseconds = Duration<DurationPeriod::Nanosecond>;
	using Microseconds = Duration<DurationPeriod::Microsecond>;
	using Milliseconds = Duration<DurationPeriod::Millisecond>;
	using Seconds = Duration<DurationPeriod::Second>;

		/// @brief Infinite timeout expressed as Nanoseconds value.
	constexpr Nanoseconds cvTimeoutInfiniteNs { CX_INT64_MAX };

	/// @brief Infinite timeout expressed as Microseconds value.
	constexpr Microseconds cvTimeoutInfiniteUs { CX_INT64_MAX };

	/// @brief Infinite timeout expressed as Milliseconds value.
	constexpr Milliseconds cvTimeoutInfiniteMs { CX_INT64_MAX };

	/// @brief Infinite timeout expressed as Seconds value.
	constexpr Seconds cvTimeoutInfiniteSec { CX_INT64_MAX };

}
