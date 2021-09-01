
#ifndef __TS3_STDEXT_BITMASK_H__
#define __TS3_STDEXT_BITMASK_H__

#include "typeTraits.h"

namespace ts3
{

	template <typename TpIntegral>
	class Bitmask
	{
		static_assert( ( std::is_integral<TpIntegral>::value || std::is_enum<TpIntegral>::value ) && !std::is_same<TpIntegral, bool>::value,
			"Atomic masks are only valid for integer and enum types (but not a bool type)!" );

	public:
		using MyType = Bitmask<TpIntegral>;
		using ValueType = typename UintTypeBySize<sizeof( TpIntegral )>::Type;

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

		constexpr operator TpIntegral() const
		{
			return static_cast<TpIntegral>( _value );
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
		void setOrUnset( bool pSet, TpBits pBits )
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
			if ( ( _value & static_cast<ValueType>( pBits ) ) != 0 )
			{
				return false;
			}

			_value |= static_cast<ValueType>( pBits );
			return true;
		}

		template <typename TpBits>
		bool testAndUnset( TpBits pBits )
		{
			if ( ( _value & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( pBits ) )
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

		ValueType get() const
		{
			return _value;
		}

		template <typename TpBits>
		ValueType test( TpBits pBits ) const
		{
			return _value & static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		constexpr bool isSet( TpBits pBits ) const
		{
			return ( _value & static_cast<ValueType>( pBits ) ) == static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		constexpr bool isSetAnyOf( TpBits pBits ) const
		{
			return ( _value & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( 0 );
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

	template <typename TpIntegral>
	inline constexpr Bitmask<TpIntegral> makeBitmask( TpIntegral pValue )
	{
	    return Bitmask<TpIntegral>( pValue );
	}

}

#endif // __TS3_STDEXT_BITMASK_H__
