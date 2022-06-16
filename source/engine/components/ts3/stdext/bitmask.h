
#ifndef __TS3_STDEXT_BITMASK_H__
#define __TS3_STDEXT_BITMASK_H__

#include "typeTraits.h"
#include <ts3/platform/gds.h>

namespace ts3
{

	template <typename TpVal>
	class Bitmask
	{
		static_assert(
			( std::is_integral<TpVal>::value || std::is_enum<TpVal>::value ) && !std::is_same<TpVal, bool>::value,
			"Atomic masks are only valid for integer and enum types (but not a bool type)!" );

	public:
		using MyType = Bitmask<TpVal>;
		using ValueType = typename UintTypeBySize<sizeof( TpVal )>::Type;

	public:
		constexpr Bitmask() noexcept
		: _value( 0 )
		{}

		template <typename TpBits>
		constexpr Bitmask( TpBits pValue ) noexcept
		: _value( static_cast<ValueType>( pValue ) )
		{}

		template <typename TpBits>
		MyType & operator=( TpBits pValue )
		{
			_value = static_cast<ValueType>( pValue );
			return *this;
		}

		constexpr operator TpVal() const
		{
			return static_cast<TpVal>( _value );
		}

		template <typename TpBits>
		void store( TpBits pBits )
		{
			_value = static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		void set( TpBits pBits )
		{
			_value |= static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		void toggle( TpBits pBits )
		{
			_value ^= static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		void unset( TpBits pBits )
		{
			_value &= ~static_cast<ValueType>( pBits );
		}

		void invert()
		{
			_value = ~( _value );
		}

		template <typename TpBits>
		void setOrUnset( TpBits pBits, bool pSet )
		{
			if( pSet )
			{
				set( pBits );
			}
			else
			{
				unset( pBits );
			}
		}

		template <typename TpBits>
		bool testAndSet( TpBits pBits )
		{
			if( ( _value & static_cast<ValueType>( pBits ) ) != 0 )
			{
				return false;
			}

			_value |= static_cast<ValueType>( pBits );
			return true;
		}

		template <typename TpBits>
		bool testAndUnset( TpBits pBits )
		{
			if( ( _value & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( pBits ) )
			{
				return false;
			}

			_value &= ~static_cast<ValueType>( pBits );
			return true;
		}

		void clear()
		{
			_value = 0;
		}

		TS3_FUNC_NO_DISCARD ValueType get() const
		{
			return _value;
		}

		template <typename TpBits>
		TS3_FUNC_NO_DISCARD ValueType test( TpBits pBits ) const
		{
			return _value & static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		TS3_FUNC_NO_DISCARD constexpr bool isSet( TpBits pBits ) const
		{
			return ( static_cast<ValueType>( pBits ) != 0 ) && ( ( _value & static_cast<ValueType>( pBits ) ) == static_cast<ValueType>( pBits ) );
		}

		template <typename TpBits>
		TS3_FUNC_NO_DISCARD constexpr bool isSetAnyOf( TpBits pBits ) const
		{
			return ( _value & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( 0 );
		}

		TS3_FUNC_NO_DISCARD constexpr bool empty() const
		{
			return _value == 0;
		}

		template <typename TpBits>
		void operator|=( TpBits pBits )
		{
			_value |= static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		void operator&=( TpBits pBits )
		{
			_value &= static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		void operator^=( TpBits pBits )
		{
			_value ^= static_cast<ValueType>( pBits );
		}

		void operator<<=( size_t pShift )
		{
			_value <<= pShift;
		}

		void operator>>=( size_t pShift )
		{
			_value <<= pShift;
		}

		ValueType operator~() const
		{
			return ~( _value );
		}

		template <typename TpBits>
		ValueType operator|( TpBits pBits ) const
		{
			return _value | static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		ValueType operator&( TpBits pBits ) const
		{
			return _value & static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		ValueType operator^( TpBits pBits ) const
		{
			return _value ^ static_cast<ValueType>( pBits );
		}

		ValueType operator<<( size_t pShift ) const
		{
			return _value << pShift;
		}

		ValueType operator>>( size_t pShift ) const
		{
			return _value >> pShift;
		}

	private:
		ValueType  _value;
	};

	template <typename TpVal>
	inline constexpr Bitmask<TpVal> makeBitmask( TpVal pValue = static_cast<TpVal>( 0 ) )
	{
		return Bitmask<TpVal>( pValue );
	}

}

#endif // __TS3_STDEXT_BITMASK_H__
