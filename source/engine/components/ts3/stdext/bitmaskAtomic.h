
#ifndef __TS3_STDEXT_BITMASK_ATOMIC_H__
#define __TS3_STDEXT_BITMASK_ATOMIC_H__

#include "prerequisites.h"
#include <atomic>

namespace ts3
{

	template <typename TVal>
	class AtomicBitmask
	{
		static_assert(
			( std::is_integral<TVal>::value || std::is_enum<TVal>::value ) && !std::is_same<TVal, bool>::value,
			"Atomic Masks are only valid for integer and enum types (but not a bool type)!" );

	public:
		using MyType = AtomicBitmask<TVal>;
		using ValueType = typename UintTypeBySize<sizeof( TVal )>::Type;

	public:
		AtomicBitmask( const AtomicBitmask & ) = delete;
		AtomicBitmask & operator=( const AtomicBitmask & ) = delete;

		AtomicBitmask()  noexcept
		: _value( 0 )
		{}

		template <typename TBits>
		AtomicBitmask( TBits pBits ) noexcept
		: _value( static_cast<ValueType>( pBits ) )
		{}

		template <typename TBits>
		MyType & operator=( TBits pBits )
		{
			_value.store( static_cast<ValueType>( pBits ), std::memory_order_relaxed );
			return *this;
		}

		operator TVal() const
		{
			return static_cast<TVal>( get() );
		}

		template <typename TBits>
		void store( TBits pBits, std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.store( static_cast<ValueType>( pBits ), pOrder );
		}

		template <typename TBits>
		void set( TBits pBits, std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.fetch_or( static_cast<ValueType>( pBits ), pOrder );
		}

		template <typename TBits>
		void toggle( TBits pBits, std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.fetch_xor( static_cast<ValueType>( pBits ), pOrder );
		}

		template <typename TBits>
		void unset( TBits pBits, std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.fetch_and( ~static_cast<ValueType>( pBits ), pOrder );
		}

		template <typename TBits>
		void setOrUnset( TBits pBits, bool pSet, std::memory_order pOrder = std::memory_order_relaxed )
		{
			if( pSet )
			{
				set( pBits, pOrder );
			}
			else
			{
				unset( pBits, pOrder );
			}
		}

		template <typename TBits>
		bool testAndSet( TBits pBits )
		{
			ValueType current = _value.load( std::memory_order_relaxed );

			while( true )
			{
				if( ( current & static_cast<ValueType>( pBits ) ) != 0 )
				{
					return false;
				}

				if( _value.compare_exchange_weak( current,
				                                  current | static_cast<ValueType>( pBits ),
				                                  std::memory_order_acq_rel,
				                                  std::memory_order_relaxed ) )
				{
					return true;
				}
			}
		}

		template <typename TBits>
		bool testAndUnset( TBits pBits )
		{
			ValueType current = _value.load( std::memory_order_relaxed );

			while( true )
			{
				if( ( current & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( pBits ) )
				{
					return false;
				}

				if( _value.compare_exchange_weak( current,
				                                  current & ~static_cast<ValueType>( pBits ),
				                                  std::memory_order_acq_rel,
				                                  std::memory_order_relaxed ) )
				{
					return true;
				}
			}
		}

		void clear( std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.store( 0, pOrder );
		}

		TS3_ATTR_NO_DISCARD ValueType get( std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return _value.load( pOrder );
		}

		template <typename TBits>
		TS3_ATTR_NO_DISCARD ValueType test( TBits pBits, std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return get( pOrder ) & static_cast<ValueType>( pBits );
		}

		template <typename TBits>
		TS3_ATTR_NO_DISCARD bool isSet( TBits pBits, std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return ( pBits != 0 ) && ( ( get( pOrder ) & static_cast<ValueType>( pBits ) ) == static_cast<ValueType>( pBits ) );
		}

		template <typename TBits>
		TS3_ATTR_NO_DISCARD bool isSetAnyOf( TBits pBits, std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return ( get( pOrder ) & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( 0 );
		}

		TS3_ATTR_NO_DISCARD bool empty( std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return get( pOrder ) == 0;
		}

		template <typename TBits>
		void operator|=( TBits pBits )
		{
			_value.fetch_or( static_cast<ValueType>( pBits ), std::memory_order_relaxed );
		}

		template <typename TBits>
		void operator&=( TBits pBits )
		{
			_value.fetch_and( static_cast<ValueType>( pBits ), std::memory_order_relaxed );
		}

		template <typename TBits>
		void operator^=( TBits pBits )
		{
			_value.fetch_xor( static_cast<ValueType>( pBits ), std::memory_order_relaxed );
		}

		ValueType operator~() const
		{
			return ~( get() );
		}

		template <typename TBits>
		ValueType operator|( TBits pBits ) const
		{
			return get() | static_cast<ValueType>( pBits );
		}

		template <typename TBits>
		ValueType operator&( TBits pBits ) const
		{
			return get() & static_cast<ValueType>( pBits );
		}

		template <typename TBits>
		ValueType operator^( TBits pBits ) const
		{
			return get() ^ static_cast<ValueType>( pBits );
		}

		ValueType operator<<( size_t pShift ) const
		{
			return get() << pShift;
		}

		ValueType operator>>( size_t pShift ) const
		{
			return get() >> pShift;
		}

	private:
		std::atomic<ValueType> _value;
	};

}

#endif // __TS3_STDEXT_BITMASK_ATOMIC_H__
