
#ifndef __TS3_STDEXT_BITMASK_H__
#define __TS3_STDEXT_BITMASK_H__

#include "bitUtils.h"
#include "typeTraits.h"
#include <ts3/platform/gds.h>

namespace ts3
{

	template <typename TVal>
	class Bitmask
	{
		static_assert(
			( std::is_integral<TVal>::value || std::is_enum<TVal>::value ) && !std::is_same<TVal, bool>::value,
			"Atomic masks are only valid for integer and enum types (but not a bool type)!" );

	public:
		using MyType = Bitmask<TVal>;
		using ValueType = typename UintTypeBySize<sizeof( TVal )>::Type;

	public:
		constexpr Bitmask() noexcept
		: _value( 0 )
		{}

		template <typename TBits>
		constexpr Bitmask( TBits pValue ) noexcept
		: _value( static_cast<ValueType>( pValue ) )
		{}

		template <typename TBits>
		MyType & operator=( TBits pValue )
		{
			_value = static_cast<ValueType>( pValue );
			return *this;
		}

		constexpr operator TVal() const
		{
			return static_cast<TVal>( _value );
		}

		template <typename TBits>
		MyType & store( TBits pBits )
		{
			_value = static_cast<ValueType>( pBits );
			return *this;
		}

		template <typename TBits>
		MyType & set( TBits pBits )
		{
			_value |= static_cast<ValueType>( pBits );
			return *this;
		}

		template <typename TBits>
		MyType & toggle( TBits pBits )
		{
			_value ^= static_cast<ValueType>( pBits );
			return *this;
		}

		template <typename TBits>
		MyType & unset( TBits pBits )
		{
			_value &= ~static_cast<ValueType>( pBits );
			return *this;
		}

		MyType & invert()
		{
			_value = ~( _value );
			return *this;
		}

		template <typename TBits>
		MyType & setOrUnset( TBits pBits, bool pSet )
		{
			pSet ? set( pBits ) : unset( pBits );
			return *this;
		}

		template <typename TBits>
		bool testAndSet( TBits pBits )
		{
			if( ( _value & static_cast<ValueType>( pBits ) ) != 0 )
			{
				return false;
			}

			_value |= static_cast<ValueType>( pBits );
			return true;
		}

		template <typename TBits>
		bool testAndUnset( TBits pBits )
		{
			if( ( _value & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( pBits ) )
			{
				return false;
			}

			_value &= ~static_cast<ValueType>( pBits );
			return true;
		}

		template <typename TBits>
		MyType & applyAnd( TBits pBits )
		{
			_value &= pBits;
			return *this;
		}

		template <typename TBits>
		MyType & applyOr( TBits pBits )
		{
			_value |= pBits;
			return *this;
		}

		template <typename TBits>
		MyType & applyXor( TBits pBits )
		{
			_value ^= pBits;
			return *this;
		}

		void clear()
		{
			_value = 0;
		}

		TS3_ATTR_NO_DISCARD ValueType get() const
		{
			return _value;
		}

		template <typename TBits>
		TS3_ATTR_NO_DISCARD ValueType test( TBits pBits ) const
		{
			return _value & static_cast<ValueType>( pBits );
		}

		template <typename TBits>
		TS3_ATTR_NO_DISCARD constexpr bool isSet( TBits pBits ) const
		{
			return ( static_cast<ValueType>( pBits ) != 0 ) && ( ( _value & static_cast<ValueType>( pBits ) ) == static_cast<ValueType>( pBits ) );
		}

		template <typename TBits>
		TS3_ATTR_NO_DISCARD constexpr bool isSetAnyOf( TBits pBits ) const
		{
			return ( _value & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( 0 );
		}

		TS3_ATTR_NO_DISCARD constexpr uint16 countBits() const
		{
			return popCount( _value );;
		}

		TS3_ATTR_NO_DISCARD constexpr bool empty() const
		{
			return _value == 0;
		}

		TS3_ATTR_NO_DISCARD static constexpr uint16 countBits( ValueType pValue )
		{
			return popCount( pValue );
		}

		template <typename TBits>
		constexpr void operator|=( TBits pBits )
		{
			_value |= static_cast<ValueType>( pBits );
		}

		template <typename TBits>
		constexpr void operator&=( TBits pBits )
		{
			_value &= static_cast<ValueType>( pBits );
		}

		template <typename TBits>
		constexpr void operator^=( TBits pBits )
		{
			_value ^= static_cast<ValueType>( pBits );
		}

		constexpr void operator<<=( size_t pShift )
		{
			_value <<= pShift;
		}

		constexpr void operator>>=( size_t pShift )
		{
			_value <<= pShift;
		}

		constexpr MyType operator~() const
		{
			return ~( _value );
		}

		template <typename TBits>
		constexpr MyType operator|( TBits pBits ) const
		{
			return _value | static_cast<ValueType>( pBits );
		}

		template <typename TBits>
		constexpr MyType operator&( TBits pBits ) const
		{
			return _value & static_cast<ValueType>( pBits );
		}

		template <typename TBits>
		constexpr MyType operator^( TBits pBits ) const
		{
			return _value ^ static_cast<ValueType>( pBits );
		}

		constexpr MyType operator<<( size_t pShift ) const
		{
			return _value << pShift;
		}

		constexpr MyType operator>>( size_t pShift ) const
		{
			return _value >> pShift;
		}

	private:
		ValueType  _value;
	};

	template <typename TEnum, typename std::enable_if<std::is_enum<TEnum>::value, int>::type = 0>
	TS3_ATTR_NO_DISCARD inline constexpr Bitmask<typename std::underlying_type<TEnum>::type> makeBitmask( TEnum pValue = static_cast<TEnum>( 0 ) ) noexcept
	{
		return Bitmask<typename std::underlying_type<TEnum>::type>( pValue );
	}

	template <typename TIntegral, typename std::enable_if<std::is_integral<TIntegral>::value, int>::type = 0>
	TS3_ATTR_NO_DISCARD inline constexpr Bitmask<TIntegral> makeBitmask( TIntegral pValue ) noexcept
	{
		return Bitmask<TIntegral>( pValue );
	}

	template <typename TVal, typename TInput>
	TS3_ATTR_NO_DISCARD inline constexpr Bitmask<TVal> makeBitmaskEx( TInput pValue = static_cast<TInput>( 0 ) ) noexcept
	{
		return Bitmask<TVal>( pValue );
	}

}

#endif // __TS3_STDEXT_BITMASK_H__
