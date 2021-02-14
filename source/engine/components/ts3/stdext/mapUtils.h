
#ifndef __TS3_STDEXT_MAP_UTILS_H__
#define __TS3_STDEXT_MAP_UTILS_H__

#include "prerequisites.h"

namespace ts3
{

	template <typename TpMap, typename TpKey = typename TpMap::key_type>
	typename TpMap::mapped_type * getMapValueOrNullptr( TpMap & pMap, const TpKey & pKey )
	{
		auto valueIter = pMap.find( pKey );
		if( valueIter != pMap.end() )
		{
			return &( valueIter->second );
		}
		return nullptr;
	}

	template <typename TpMap, typename TpKey = typename TpMap::key_type>
	const typename TpMap::mapped_type * getMapValueOrNullptr( const TpMap & pMap, const TpKey & pKey )
	{
		auto valueIter = pMap.find( pKey );
		if( valueIter != pMap.end() )
		{
			return &( valueIter->second );
		}
		return nullptr;
	}

	template <typename TpMap, typename TpKey = typename TpMap::key_type>
	typename TpMap::mapped_type & getMapValueOrDefault( const TpMap & pMap, const TpKey & pKey, typename TpMap::mapped_type & pDefault )
	{
		auto valueIter = pMap.find( pKey );
		if( valueIter != pMap.end() )
		{
			return valueIter->second;
		}
		return pDefault;
	}

	template <typename TpMap, typename TpKey = typename TpMap::key_type>
	const typename TpMap::mapped_type & getMapValueOrDefault( const TpMap & pMap, const TpKey & pKey, const typename TpMap::mapped_type & pDefault )
	{
		auto valueIter = pMap.find( pKey );
		if( valueIter != pMap.end() )
		{
			return valueIter->second;
		}
		return pDefault;
	}

}

#endif // __TS3_STDEXT_MAP_UTILS_H__
