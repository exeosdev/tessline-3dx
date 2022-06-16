
#ifndef __TS3_STDEXT_BITMASK_ATOMIC_H__
#define __TS3_STDEXT_BITMASK_ATOMIC_H__

#include "prerequisites.h"
#include <atomic>

namespace ts3
{

	template <typename TpVal>
	class AtomicBitmask
	{
		static_assert(
			( std::is_integral<TpVal>::value || std::is_enum<TpVal>::value ) && !std::is_same<TpVal, bool>::value,
			"Atomic Masks are only valid for integer and enum types (but not a bool type)!" );

	public:
		using MyType = AtomicBitmask<TpVal>;
		using ValueType = typename UintTypeBySize<sizeof( TpVal )>::Type;

	public:
		AtomicBitmask( const AtomicBitmask & ) = delete;
		AtomicBitmask & operator=( const AtomicBitmask & ) = delete;

		AtomicBitmask()  noexcept
		: _value( 0 )
		{}

		template <typename TpBits>
		AtomicBitmask( TpBits pBits ) noexcept
		: _value( static_cast<ValueType>( pBits ) )
		{}

		template <typename TpBits>
		MyType & operator=( TpBits pBits )
		{
			_value.store( static_cast<ValueType>( pBits ), std::memory_order_relaxed );
			return *this;
		}

		operator TpVal() const
		{
			return static_cast<TpVal>( get() );
		}

		template <typename TpBits>
		void store( TpBits pBits, std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.store( static_cast<ValueType>( pBits ), pOrder );
		}

		template <typename TpBits>
		void set( TpBits pBits, std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.fetch_or( static_cast<ValueType>( pBits ), pOrder );
		}

		template <typename TpBits>
		void toggle( TpBits pBits, std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.fetch_xor( static_cast<ValueType>( pBits ), pOrder );
		}

		template <typename TpBits>
		void unset( TpBits pBits, std::memory_order pOrder = std::memory_order_relaxed )
		{
			_value.fetch_and( ~static_cast<ValueType>( pBits ), pOrder );
		}

		template <typename TpBits>
		void setOrUnset( TpBits pBits, bool pSet, std::memory_order pOrder = std::memory_order_relaxed )
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

		template <typename TpBits>
		bool testAndSet( TpBits pBits )
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

		template <typename TpBits>
		bool testAndUnset( TpBits pBits )
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

		TS3_FUNC_NO_DISCARD ValueType get( std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return _value.load( pOrder );
		}

		template <typename TpBits>
		TS3_FUNC_NO_DISCARD ValueType test( TpBits pBits, std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return get( pOrder ) & static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		TS3_FUNC_NO_DISCARD bool isSet( TpBits pBits, std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return ( get( pOrder ) & static_cast<ValueType>( pBits ) ) == static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		TS3_FUNC_NO_DISCARD bool isSetAnyOf( TpBits pBits, std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return ( get( pOrder ) & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( 0 );
		}

		TS3_FUNC_NO_DISCARD bool empty( std::memory_order pOrder = std::memory_order_relaxed ) const
		{
			return get( pOrder ) == 0;
		}

		template <typename TpBits>
		void operator|=( TpBits pBits )
		{
			_value.fetch_or( static_cast<ValueType>( pBits ), std::memory_order_relaxed );
		}

		template <typename TpBits>
		void operator&=( TpBits pBits )
		{
			_value.fetch_and( static_cast<ValueType>( pBits ), std::memory_order_relaxed );
		}

		template <typename TpBits>
		void operator^=( TpBits pBits )
		{
			_value.fetch_xor( static_cast<ValueType>( pBits ), std::memory_order_relaxed );
		}

		ValueType operator~() const
		{
			return ~( get() );
		}

		template <typename TpBits>
		ValueType operator|( TpBits pBits ) const
		{
			return get() | static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		ValueType operator&( TpBits pBits ) const
		{
			return get() & static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		ValueType operator^( TpBits pBits ) const
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
