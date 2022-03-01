
#ifndef __TS3_STDEXT_BIT_UTILS_H__
#define __TS3_STDEXT_BIT_UTILS_H__

#include "prerequisites.h"
#include <climits>

namespace ts3
{

	template <typename TpUint, size_t tShift, typename TpInput>
	inline constexpr TpUint bshLeft( TpInput pValue )
	{
		static_assert( sizeof( TpUint ) >= sizeof( TpInput ), "Target uint type has smaller size than the input, explicit cast required" );
		static_assert( tShift <= sizeof( TpUint ) * CHAR_BIT, "Bit shift outside the integer range is an Undefined Behaviour" );
		return static_cast<TpUint>( pValue ) << tShift;
	}

	template <typename TpUint, size_t tShift, typename TpInput>
	inline constexpr TpUint bshRight( TpInput pValue )
	{
		static_assert( sizeof( TpUint ) >= sizeof( TpInput ), "Target uint type has smaller size than the input, explicit cast required" );
		static_assert( tShift <= sizeof( TpUint ) * CHAR_BIT, "Bit shift outside the integer range is an Undefined Behaviour" );
		return static_cast<TpUint>( pValue ) >> tShift;
	}

}

#endif // __TS3_STDEXT_BIT_UTILS_H__
