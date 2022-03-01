
#pragma once

#include "prerequisites.h"
#include <chrono>

namespace ts3
{

	using duration_value_t = uint64;

	enum class EDurationPeriod : enum_default_value_t
	{
		Nanosecond,
		Microsecond,
		Millisecond,
		Second
	};

	template <EDurationPeriod>
	struct DurationTypeWrapper;

	template <>
	struct DurationTypeWrapper<EDurationPeriod::Nanosecond>
	{
		using Type = std::chrono::nanoseconds;
	};

	template <>
	struct DurationTypeWrapper<EDurationPeriod::Microsecond>
	{
		using Type = std::chrono::microseconds;
	};

	template <>
	struct DurationTypeWrapper<EDurationPeriod::Millisecond>
	{
		using Type = std::chrono::milliseconds;
	};

	template <>
	struct DurationTypeWrapper<EDurationPeriod::Second>
	{
		using Type = std::chrono::seconds;
	};


	template <EDurationPeriod tpPeriod>
	using Duration = typename DurationTypeWrapper<tpPeriod>::Type;


	template <EDurationPeriod tpPeriod>
	struct DurationTraits
	{
		using RatioType = typename DurationTypeWrapper<tpPeriod>::Type::period;

		static constexpr std::pair<native_int, native_int> unitRatio {
			static_cast<native_int>( RatioType::num ),
			static_cast<native_int>( RatioType::den )
		};
	};

	using Nanoseconds = Duration<EDurationPeriod::Nanosecond>;
	using Microseconds = Duration<EDurationPeriod::Microsecond>;
	using Milliseconds = Duration<EDurationPeriod::Millisecond>;
	using Seconds = Duration<EDurationPeriod::Second>;

	/// @brief Infinite timeout expressed as Nanoseconds value.
	inline constexpr Nanoseconds cvTimeoutInfiniteNs { CX_INT64_MAX };

	/// @brief Infinite timeout expressed as Microseconds value.
	inline constexpr Microseconds cvTimeoutInfiniteUs { CX_INT64_MAX };

	/// @brief Infinite timeout expressed as Milliseconds value.
	inline constexpr Milliseconds cvTimeoutInfiniteMs { CX_INT64_MAX };

	/// @brief Infinite timeout expressed as Seconds value.
	inline constexpr Seconds cvTimeoutInfiniteSec { CX_INT64_MAX };

}
