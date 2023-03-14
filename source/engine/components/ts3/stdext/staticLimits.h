
#ifndef __TS3_STDEXT_STATIC_LIMITS_H__
#define __TS3_STDEXT_STATIC_LIMITS_H__

#include "prerequisites.h"

#include <cfloat>
#include <climits>
#include <numeric>
#include <type_traits>

namespace ts3
{

	template <typename TType, size_t tTypeSize, bool tSigned>
	struct IntegerLimitsProxy;

	template <typename TType>
	struct IntegerLimitsProxy<TType, 1, true>
	{
		static constexpr uint32 bitCount = CHAR_BIT;
		static constexpr TType minValue = INT8_MIN;
		static constexpr TType maxValue = INT8_MAX;
		static constexpr TType epsilon = 0;
		static constexpr TType roundError = 0;
	};

	template <typename TType>
	struct IntegerLimitsProxy<TType, 1, false>
	{
		static constexpr uint32 bitCount = CHAR_BIT;
		static constexpr TType minValue = 0U;
		static constexpr TType maxValue = UINT8_MAX;
		static constexpr TType epsilon = 0U;
		static constexpr TType roundError = 0U;
	};

	template <typename TType>
	struct IntegerLimitsProxy<TType, 2, true>
	{
		static constexpr uint32 bitCount = CHAR_BIT * 2;
		static constexpr TType minValue = INT16_MIN;
		static constexpr TType maxValue = INT16_MAX;
		static constexpr TType epsilon = 0;
		static constexpr TType roundError = 0;
	};

	template <typename TType>
	struct IntegerLimitsProxy<TType, 2, false>
	{
		static constexpr uint32 bitCount = CHAR_BIT * 2;
		static constexpr TType minValue = 0U;
		static constexpr TType maxValue = UINT16_MAX;
		static constexpr TType epsilon = 0U;
		static constexpr TType roundError = 0U;
	};

	template <typename TType>
	struct IntegerLimitsProxy<TType, 4, true>
	{
		static constexpr uint32 bitCount = CHAR_BIT * 4;
		static constexpr TType minValue = INT32_MIN;
		static constexpr TType maxValue = INT32_MAX;
		static constexpr TType epsilon = 0;
		static constexpr TType roundError = 0;
	};

	template <typename TType>
	struct IntegerLimitsProxy<TType, 4, false>
	{
		static constexpr uint32 bitCount = CHAR_BIT * 4;
		static constexpr TType minValue = 0U;
		static constexpr TType maxValue = UINT32_MAX;
		static constexpr TType epsilon = 0U;
		static constexpr TType roundError = 0U;
	};

	template <typename TType>
	struct IntegerLimitsProxy<TType, 8, true>
	{
		static constexpr uint32 bitCount = CHAR_BIT * 8;
		static constexpr TType minValue = LLONG_MIN;
		static constexpr TType maxValue = LLONG_MAX;
		static constexpr TType epsilon = 0;
		static constexpr TType roundError = 0;
	};

	template <typename TType>
	struct IntegerLimitsProxy<TType, 8, false>
	{
		static constexpr uint32 bitCount = CHAR_BIT * 8;
		static constexpr TType minValue = 0U;
		static constexpr TType maxValue = ULLONG_MAX;
		static constexpr TType epsilon = 0U;
		static constexpr TType roundError = 0U;
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

	constexpr auto CX_INT16_MAX = Limits<int16>::maxValue;
	constexpr auto CX_INT16_MIN = Limits<int16>::minValue;
	constexpr auto CX_UINT16_MAX = Limits<uint16>::maxValue;
	constexpr auto CX_UINT16_MIN = Limits<uint16>::minValue;

	constexpr auto CX_INT32_MAX = Limits<int32>::maxValue;
	constexpr auto CX_INT32_MIN = Limits<int32>::minValue;
	constexpr auto CX_UINT32_MAX = Limits<uint32>::maxValue;
	constexpr auto CX_UINT32_MIN = Limits<uint32>::minValue;

	constexpr auto CX_INT64_MAX = Limits<int64>::maxValue;
	constexpr auto CX_INT64_MIN = Limits<int64>::minValue;
	constexpr auto CX_UINT64_MAX = Limits<uint64>::maxValue;
	constexpr auto CX_UINT64_MIN = Limits<uint64>::minValue;

}

#endif /* __TS3_STDEXT_STATIC_LIMITS_H__ */
