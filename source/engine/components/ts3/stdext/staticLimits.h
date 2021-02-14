
#ifndef __TS3_STDEXT_STATIC_ALGO_H__
#define __TS3_STDEXT_STATIC_ALGO_H__

#include "prerequisites.h"
#include <cfloat>
#include <climits>
#include <numeric>
#include <type_traits>

namespace ts3
{

	template <typename TpType, size_t tTypeSize, bool tSigned>
	struct IntegerLimitsProxy;

	template <typename TpType>
	struct IntegerLimitsProxy<TpType, 1, true>
	{
		static constexpr TpType minValue = INT8_MIN;
		static constexpr TpType maxValue = INT8_MAX;
		static constexpr TpType epsilon = 0;
		static constexpr TpType roundError = 0;
	};

	template <typename TpType>
	struct IntegerLimitsProxy<TpType, 1, false>
	{
		static constexpr TpType minValue = 0U;
		static constexpr TpType maxValue = UINT8_MAX;
		static constexpr TpType epsilon = 0U;
		static constexpr TpType roundError = 0U;
	};

	template <typename TpType>
	struct IntegerLimitsProxy<TpType, 2, true>
	{
		static constexpr TpType minValue = INT16_MIN;
		static constexpr TpType maxValue = INT16_MAX;
		static constexpr TpType epsilon = 0;
		static constexpr TpType roundError = 0;
	};

	template <typename TpType>
	struct IntegerLimitsProxy<TpType, 2, false>
	{
		static constexpr TpType minValue = 0U;
		static constexpr TpType maxValue = UINT16_MAX;
		static constexpr TpType epsilon = 0U;
		static constexpr TpType roundError = 0U;
	};

	template <typename TpType>
	struct IntegerLimitsProxy<TpType, 4, true>
	{
		static constexpr TpType minValue = INT32_MIN;
		static constexpr TpType maxValue = INT32_MAX;
		static constexpr TpType epsilon = 0;
		static constexpr TpType roundError = 0;
	};

	template <typename TpType>
	struct IntegerLimitsProxy<TpType, 4, false>
	{
		static constexpr TpType minValue = 0U;
		static constexpr TpType maxValue = UINT32_MAX;
		static constexpr TpType epsilon = 0U;
		static constexpr TpType roundError = 0U;
	};

	template <typename TpType>
	struct IntegerLimitsProxy<TpType, 8, true>
	{
		static constexpr TpType minValue = LLONG_MIN;
		static constexpr TpType maxValue = LLONG_MAX;
		static constexpr TpType epsilon = 0;
		static constexpr TpType roundError = 0;
	};

	template <typename TpType>
	struct IntegerLimitsProxy<TpType, 8, false>
	{
		static constexpr TpType minValue = 0U;
		static constexpr TpType maxValue = ULLONG_MAX;
		static constexpr TpType epsilon = 0U;
		static constexpr TpType roundError = 0U;
	};

	template <typename>
	struct Limits;

	template <>
	struct Limits<char> : public IntegerLimitsProxy<char, sizeof( char ), std::is_signed<char>::value>
	{
		using ValueType = char;
	};

	template <>
	struct Limits<signed char> : public IntegerLimitsProxy<signed char, sizeof( signed char ), true>
	{
		using ValueType = signed char;
	};

	template <>
	struct Limits<unsigned char> : public IntegerLimitsProxy<unsigned char, sizeof( unsigned char ), false>
	{
		using ValueType = unsigned char;
	};

	template <>
	struct Limits<signed short> : public IntegerLimitsProxy<signed short, sizeof( signed short ), true>
	{
		using ValueType = signed short;
	};

	template <>
	struct Limits<unsigned short> : public IntegerLimitsProxy<unsigned short, sizeof( unsigned short ), false>
	{
		using ValueType = unsigned short;
	};

	template <>
	struct Limits<signed int> : public IntegerLimitsProxy<signed int, sizeof( signed int ), true>
	{
		using ValueType = signed int;
	};

	template <>
	struct Limits<unsigned int> : public IntegerLimitsProxy<unsigned int, sizeof( unsigned int ), false>
	{
		using ValueType = unsigned int;
	};

	template <>
	struct Limits<signed long> : public IntegerLimitsProxy<signed long, sizeof( signed long ), true>
	{
		using ValueType = signed long;
	};

	template <>
	struct Limits<unsigned long> : public IntegerLimitsProxy<unsigned long, sizeof( unsigned long ), false>
	{
		using ValueType = unsigned long;
	};

	template <>
	struct Limits<signed long long> : public IntegerLimitsProxy<signed long long, sizeof( signed long long ), true>
	{
		using ValueType = signed long long;
	};

	template <>
	struct Limits<unsigned long long> : public IntegerLimitsProxy<unsigned long long, sizeof( unsigned long long ), false>
	{
		using ValueType = unsigned long long;
	};

	template <>
	struct Limits<float>
	{
		using ValueType = float;

		static constexpr ValueType minValue = FLT_MIN;
		static constexpr ValueType maxValue = FLT_MAX;
		static constexpr ValueType epsilon = 0;
		static constexpr ValueType roundError = 0.5f;
	};

	template <>
	struct Limits<double>
	{
		using ValueType = double;

		static constexpr ValueType minValue = DBL_MIN;
		static constexpr ValueType maxValue = DBL_MAX;
		static constexpr ValueType epsilon = DBL_EPSILON;
		static constexpr ValueType roundError = 0.5;
	};

	template <>
	struct Limits<long double>
	{
		using ValueType = long double;

		static constexpr ValueType minValue = LDBL_MIN;
		static constexpr ValueType maxValue = LDBL_MAX;
		static constexpr ValueType epsilon = LDBL_EPSILON;
		static constexpr ValueType roundError = 0.5L;
	};

	inline constexpr auto cxInt16Max = Limits<int16>::maxValue;
	inline constexpr auto cxInt16Min = Limits<int16>::minValue;
	inline constexpr auto cxUint16Max = Limits<uint16>::maxValue;
	inline constexpr auto cxUint16Min = Limits<uint16>::minValue;

	inline constexpr auto cxInt32Max = Limits<int32>::maxValue;
	inline constexpr auto cxInt32Min = Limits<int32>::minValue;
	inline constexpr auto cxUint32Max = Limits<uint32>::maxValue;
	inline constexpr auto cxUint32Min = Limits<uint32>::minValue;

	inline constexpr auto cxInt64Max = Limits<int64>::maxValue;
	inline constexpr auto cxInt64Min = Limits<int64>::minValue;
	inline constexpr auto cxUint64Max = Limits<uint64>::maxValue;
	inline constexpr auto cxUint64Min = Limits<uint64>::minValue;

}

#endif /* __TS3_STDEXT_STATIC_ALGO_H__ */
