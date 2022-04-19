
#ifndef __TS3_STDEXT_BITMASK_ATOMIC_H__
#define __TS3_STDEXT_BITMASK_ATOMIC_H__

#include "prerequisites.h"
#include <atomic>

namespace ts3
{

	template <typename TpIntegral>
	class AtomicBitmask
	{
		static_assert( ( std::is_integral<TpIntegral>::value || std::is_enum<TpIntegral>::value ) && !std::is_same<TpIntegral, bool>::value,
		               "Atomic Masks are only valid for integer and enum types (but not a bool type)!" );

	public:
		using MyType = AtomicBitmask<TpIntegral>;
		using ValueType = typename UintTypeBySize<sizeof( TpIntegral )>::Type;

	public:
		AtomicBitmask( const AtomicBitmask & ) = delete;
		AtomicBitmask & operator=( const AtomicBitmask & ) = delete;

		AtomicBitmask()  noexcept
		: _value( 0 )
		{}

		template <typename TpBits>
		AtomicBitmask( TpBits pBits ) noexcept
		: _value( static_cast<ValueType>(pBits) )
		{}

		template <typename TpBits>
		MyType & operator=( TpBits pBits )
		{
			_value.store( static_cast<ValueType>(pBits), std::memory_order_relaxed );
			return *this;
		}

		operator ValueType() const
		{
			return get();
		}

		template <typename TpBits>
		void store( TpBits pBits )
		{
			_value.store( static_cast<ValueType>(pBits), std::memory_order_relaxed );
		}

		template <typename TpBits>
		void store( TpBits pBits, std::memory_order pOrder )
		{
			_value.store( static_cast<ValueType>(pBits), pOrder );
		}

		template <typename TpBits>
		void set( TpBits pBits )
		{
			_value.fetch_or( static_cast<ValueType>(pBits), std::memory_order_relaxed );
		}

		template <typename TpBits>
		void set( TpBits pBits, std::memory_order pOrder )
		{
			_value.fetch_or( static_cast<ValueType>(pBits), pOrder );
		}

		template <typename TpBits>
		void toggle( TpBits pBits )
		{
			_value.fetch_xor( static_cast<ValueType>(pBits), std::memory_order_relaxed );
		}

		template <typename TpBits>
		void toggle( TpBits pBits, std::memory_order pOrder )
		{
			_value.fetch_xor( static_cast<ValueType>(pBits), pOrder );
		}

		template <typename TpBits>
		void unset( TpBits pBits )
		{
			_value.fetch_and( ~static_cast<ValueType>(pBits), std::memory_order_relaxed );
		}

		template <typename TpBits>
		void unset( TpBits pBits, std::memory_order pOrder )
		{
			_value.fetch_and( ~static_cast<ValueType>(pBits), pOrder );
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
		void setOrUnset( TpBits pBits, bool pSet, std::memory_order pOrder )
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

		void clear()
		{
			_value.store( 0, std::memory_order_relaxed );
		}

		void clear( std::memory_order pOrder )
		{
			_value.store( 0, pOrder );
		}

		template <typename TpBits>
		bool testAndSet( TpBits pBits )
		{
			ValueType current = _value.load( std::memory_order_relaxed );

			while( true )
			{
				if( ( current & static_cast<ValueType>(pBits) ) != 0 )
				{
					return false;
				}

				if( _value.compare_exchange_weak( current, current | static_cast<ValueType>(pBits),
												   std::memory_order_acq_rel, std::memory_order_relaxed ) )
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
				if( ( current & static_cast<ValueType>(pBits) ) != static_cast<ValueType>(pBits) )
				{
					return false;
				}

				if( _value.compare_exchange_weak( current, current & ~static_cast<ValueType>(pBits),
												   std::memory_order_acq_rel, std::memory_order_relaxed ) )
				{
					return true;
				}
			}
		}

		ValueType get() const
		{
			return _value.load( std::memory_order_relaxed );
		}

		ValueType get( std::memory_order pOrder ) const
		{
			return _value.load( pOrder );
		}

		template <typename TpBits>
		ValueType test( TpBits pBits ) const
		{
			return get() & static_cast<ValueType>(pBits);
		}

		template <typename TpBits>
		ValueType test( TpBits pBits, std::memory_order pOrder ) const
		{
			return get( pOrder ) & static_cast<ValueType>(pBits);
		}

		template <typename TpBits>
		bool isSet( TpBits pBits ) const
		{
			return ( get() & static_cast<ValueType>(pBits) ) == static_cast<ValueType>(pBits);
		}

		template <typename TpBits>
		bool isSet( TpBits pBits, std::memory_order pOrder ) const
		{
			return ( get( pOrder ) & static_cast<ValueType>(pBits) ) == static_cast<ValueType>(pBits);
		}

		template <typename TpBits>
		bool isSetAnyOf( TpBits pBits ) const
		{
			return ( get() & static_cast<ValueType>( pBits ) ) == static_cast<ValueType>( pBits );
		}

		template <typename TpBits>
		bool isSetAnyOf( TpBits pBits, std::memory_order pOrder ) const
		{
			return ( get( pOrder ) & static_cast<ValueType>( pBits ) ) != static_cast<ValueType>( 0 );
		}

		template <typename TpBits>
		void operator|=( TpBits pBits )
		{
			_value.fetch_or( static_cast<ValueType>(pBits), std::memory_order_relaxed );
		}

		template <typename TpBits>
		void operator&=( TpBits pBits )
		{
			_value.fetch_and( static_cast<ValueType>(pBits), std::memory_order_relaxed );
		}

		template <typename TpBits>
		void operator^=( TpBits pBits )
		{
			_value.fetch_xor( static_cast<ValueType>(pBits), std::memory_order_relaxed );
		}

		ValueType operator~() const
		{
			return ~( get() );
		}

		template <typename TpBits>
		ValueType operator|( TpBits pBits ) const
		{
			return get() | static_cast<ValueType>(pBits);
		}

		template <typename TpBits>
		ValueType operator&( TpBits pBits ) const
		{
			return get() & static_cast<ValueType>(pBits);
		}

		template <typename TpBits>
		ValueType operator^( TpBits pBits ) const
		{
			return get() ^ static_cast<ValueType>(pBits);
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
