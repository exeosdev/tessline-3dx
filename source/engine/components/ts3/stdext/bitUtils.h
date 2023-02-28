
#ifndef __TS3_STDEXT_BIT_UTILS_H__
#define __TS3_STDEXT_BIT_UTILS_H__

#include "staticLimits.h"
#include "typeTraits.h"
#include <climits>

namespace ts3
{

	template <typename TUint, size_t tShift, typename TInput>
	inline constexpr TUint bshLeft( TInput pValue )
	{
		static_assert( sizeof( TUint ) >= sizeof( TInput ), "Target uint type has smaller size than the input, explicit cast required" );
		static_assert( tShift <= sizeof( TUint ) * CHAR_BIT, "Bit shift outside the integer range is an Undefined Behaviour" );
		return static_cast<TUint>( pValue ) << tShift;
	}

	template <typename TUint, size_t tShift, typename TInput>
	inline constexpr TUint bshRight( TInput pValue )
	{
		static_assert( sizeof( TUint ) >= sizeof( TInput ), "Target uint type has smaller size than the input, explicit cast required" );
		static_assert( tShift <= sizeof( TUint ) * CHAR_BIT, "Bit shift outside the integer range is an Undefined Behaviour" );
		return static_cast<TUint>( pValue ) >> tShift;
	}

	inline constexpr uint16 popCount( uint8 pValue )
	{
		return TS3_PCL_POPCOUNT16( static_cast<native_uint>( pValue ) );
	}

	inline constexpr uint16 popCount( uint16 pValue )
	{
		return TS3_PCL_POPCOUNT16( pValue );
	}

	inline constexpr uint16 popCount( uint32 pValue )
	{
		return TS3_PCL_POPCOUNT32( pValue );
	}

	inline constexpr uint16 popCount( uint64 pValue )
	{
		return TS3_PCL_POPCOUNT64( pValue );
	}

	template <typename TVal>
	inline constexpr TVal makeLSFBitmask( size_t pBitCount )
	{
		return ( pBitCount < Limits<TVal>::bitCount ) ? static_cast<TVal>( ( 1u << pBitCount ) - 1 ) : Limits<TVal>::maxValue;
	}

	template <size_t tBitCount>
	inline constexpr typename UintTypeByBits<tBitCount>::Type makeLSFBitmask()
	{
		using TVal = typename UintTypeByBits<tBitCount>::Type;
		return ( tBitCount < Limits<TVal>::bitCount ) ? static_cast<TVal>( ( 1u << tBitCount ) - 1 ) : Limits<TVal>::maxValue;
	}

}

#endif // __TS3_STDEXT_BIT_UTILS_H__
