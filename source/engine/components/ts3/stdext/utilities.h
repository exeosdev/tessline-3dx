
#ifndef __TS3_STDEXT_UTILITIES_H__
#define __TS3_STDEXT_UTILITIES_H__

#include "prerequisites.h"
#include <array>

namespace ts3
{

	template <typename _Type, typename _Member>
	inline uint32 memberOffset( _Member _Type::* pMptr )
	{
		return static_cast<uint32>( reinterpret_cast<byte *>( &( static_cast<_Type *>( nullptr )->*pMptr ) ) - static_cast<byte *>( nullptr ) );
	}


	template <typename _Value, size_t tSize, typename _Index>
	inline constexpr _Value & staticArrayElement( _Value( &pArray )[tSize], _Index pIndex )
	{
		return pArray[static_cast<size_t>( pIndex )];
	}

	template <typename _Value, size_t tSize, typename _Index>
	inline constexpr const _Value & staticArrayElement( const _Value( &pArray )[tSize], _Index pIndex )
	{
		return pArray[static_cast<size_t>( pIndex )];
	}

	template <typename _Value, size_t tSize, typename _Index>
	inline constexpr _Value & staticArrayElement( std::array<_Value, tSize> & pArray, _Index pIndex )
	{
		return pArray[static_cast<size_t>( pIndex )];
	}

	template <typename _Value, size_t tSize, typename _Index>
	inline constexpr const _Value & staticArrayElement( const std::array<_Value, tSize> & pArray, _Index pIndex )
	{
		return pArray[static_cast<size_t>( pIndex )];
	}

	template <typename _Value, size_t tSize, typename _Index>
	inline constexpr const _Value & staticArrayElementOrDefault( const _Value( &pArray )[tSize], _Index pIndex, const _Value & pDefault )
	{
		return ( static_cast<size_t>( pIndex ) < tSize ) ? pArray[static_cast<size_t>( pIndex )] : pDefault;
	}

	template <typename _Value, size_t tSize, typename _Index>
	inline constexpr const _Value & staticArrayElementOrDefault( const std::array<_Value, tSize> & pArray, _Index pIndex, const _Value & pDefault )
	{
		return ( static_cast<size_t>( pIndex ) < tSize ) ? pArray[static_cast<size_t>( pIndex )] : pDefault;
	}

	template <typename _Value, size_t tSize>
	inline constexpr size_t staticArraySize( const _Value( &pArray )[tSize] )
	{
		return tSize;
	}

	template <typename _Value, size_t tSize>
	inline constexpr size_t staticArraySize( const std::array<_Value, tSize> & pArray )
	{
		return tSize;
	}


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

}

#endif // __TS3_STDEXT_UTILITIES_H__
