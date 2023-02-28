
#ifndef __TS3_STDEXT_UTILITIES_H__
#define __TS3_STDEXT_UTILITIES_H__

#include "prerequisites.h"
#include <array>

namespace ts3
{

	template <typename _First, typename _Second = _First>
	inline constexpr typename std::common_type<_First, _Second>::type getMaxOf( const _First & pFirst, const _Second & pSecond )
	{
		return ( pFirst >= pSecond ) ? pFirst : pSecond;
	}

	template <typename _T0, typename _T1, typename ..._Rest>
	inline constexpr typename std::common_type<_T0, _T1, _Rest...>::type getMaxOf( const _T0 & p0, const _T1 & p1, _Rest && ...pRest )
	{
		return getMaxOf( getMaxOf( p0, p1 ), std::forward<_Rest>( pRest )... );
	}


	template <typename _First, typename _Second = _First>
	inline constexpr typename std::common_type<_First, _Second>::type getMinOf( const _First & pFirst, const _Second & pSecond )
	{
		return pFirst <= pSecond ? pFirst : pSecond;
	}

	template <typename _T0, typename _T1, typename ..._Rest>
	inline constexpr typename std::common_type<_T0, _T1, _Rest...>::type getMinOf( const _T0 & p0, const _T1 & p1, _Rest && ...pRest )
	{
		return getMinOf( getMinOf( p0, p1 ), std::forward<_Rest>( pRest )... );
	}


	template <typename _Type, typename _Member>
	inline uint32 memberOffset( _Member _Type::* pMptr )
	{
		return static_cast<uint32>( reinterpret_cast<byte *>( &( static_cast<_Type *>( nullptr )->*pMptr ) ) - static_cast<byte *>( nullptr ) );
	}


	template <typename TVal, size_t tSize, typename _Index>
	inline constexpr TVal & staticArrayElement( TVal( &pArray )[tSize], _Index pIndex )
	{
		return pArray[static_cast<size_t>( pIndex )];
	}

	template <typename TVal, size_t tSize, typename _Index>
	inline constexpr const TVal & staticArrayElement( const TVal( &pArray )[tSize], _Index pIndex )
	{
		return pArray[static_cast<size_t>( pIndex )];
	}

	template <typename TVal, size_t tSize, typename _Index>
	inline constexpr TVal & staticArrayElement( std::array<TVal, tSize> & pArray, _Index pIndex )
	{
		return pArray[static_cast<size_t>( pIndex )];
	}

	template <typename TVal, size_t tSize, typename _Index>
	inline constexpr const TVal & staticArrayElement( const std::array<TVal, tSize> & pArray, _Index pIndex )
	{
		return pArray[static_cast<size_t>( pIndex )];
	}

	template <typename TVal, size_t tSize, typename _Index>
	inline constexpr const TVal & staticArrayElementOrDefault( const TVal( &pArray )[tSize], _Index pIndex, const TVal & pDefault )
	{
		return ( static_cast<size_t>( pIndex ) < tSize ) ? pArray[static_cast<size_t>( pIndex )] : pDefault;
	}

	template <typename TVal, size_t tSize, typename _Index>
	inline constexpr const TVal & staticArrayElementOrDefault( const std::array<TVal, tSize> & pArray, _Index pIndex, const TVal & pDefault )
	{
		return ( static_cast<size_t>( pIndex ) < tSize ) ? pArray[static_cast<size_t>( pIndex )] : pDefault;
	}

	template <typename TVal, size_t tSize>
	inline constexpr size_t staticArraySize( const TVal( &pArray )[tSize] )
	{
		return tSize;
	}

	template <typename TVal, size_t tSize>
	inline constexpr size_t staticArraySize( const std::array<TVal, tSize> & pArray )
	{
		return tSize;
	}


	template <typename TVal, size_t tSize, typename TOther>
	inline constexpr size_t copyArrayValues( TVal( &pTarget )[tSize], const TOther( &pSource )[tSize], size_t pOffset = 0, size_t pCount = tSize )
	{
		const auto offset = getMinOf( pOffset, tSize );
		const auto count = getMinOf( pCount, tSize - offset );

		for( size_t index = offset; index < offset + count; ++index )
		{
			pTarget[index] = pSource[index];
		}

		return count;
	}


	template <typename TVal, size_t tSize, typename TOther>
	inline constexpr size_t setArrayValues( TVal( &pArray )[tSize], TOther pValue, size_t pOffset = 0, size_t pCount = tSize )
	{
		const auto offset = getMinOf( pOffset, tSize );
		const auto count = getMinOf( pCount, tSize - offset );

		for( size_t index = offset; index < offset + count; ++index )
		{
			pArray[index] = pValue;
		}

		return count;
	}

	template <typename TVal, size_t tSize, typename TOther>
	inline constexpr size_t setArrayValues( std::array<TVal, tSize> & pArray, TOther pValue, size_t pOffset = 0, size_t pCount = tSize  )
	{
		const auto offset = getMinOf( pOffset, tSize );
		const auto count = getMinOf( pCount, tSize - offset );

		for( size_t index = offset; index < offset + count; ++index )
		{
			pArray[index] = pValue;
		}

		return count;
	}

	template <typename TIterable, typename TCallable>
	inline void foreachIn( TIterable && pIterable, TCallable pCallable )
	{
		for( auto && element : pIterable )
		{
			pCallable( element );
		}
	}

	template <typename TIterable, typename TValue, typename TMemberFunc, typename... TArgs>
	inline void foreachIn( TIterable && pIterable, TMemberFunc TValue::* pMemberFunc, TArgs && ...pArgs )
	{
		for( auto && element : pIterable )
		{
			( element.*pMemberFunc )( std::forward<TArgs>( pArgs )... );
		}
	}

	template <typename TIter, typename TValue, typename TMemberFunc, typename... TArgs>
	inline void foreachIn( TIter pBegin, TIter pEnd, TMemberFunc TValue::* pMemberFunc, TArgs && ...pArgs )
	{
		for( auto elemIter = pBegin; elemIter != pEnd; ++elemIter )
		{
			( elemIter->*pMemberFunc )( std::forward<TArgs>( pArgs )... );
		}
	}

	template <typename TIter>
	inline size_t countNonNullPointers( TIter pBegin, TIter pEnd )
	{
		const auto numElements = std::count_if( pBegin, pEnd, []( auto && pValue ) -> bool { return !!pValue; } );
	}

	template <typename TIter>
	inline size_t countNullPointers( TIter pBegin, TIter pEnd )
	{
		const auto numElements = std::count_if( pBegin, pEnd, []( auto && pValue ) -> bool { return !pValue; } );
	}

}

#endif // __TS3_STDEXT_UTILITIES_H__
