
#pragma once

#ifndef __TS3_CORE_CORE_DEFS_H__
#define __TS3_CORE_CORE_DEFS_H__

#include <memory>
#include <vector>

#define _ts3MakeStr2( pValue ) #pValue

#define ts3MakeStr( pValue ) _ts3MakeStr2( pValue )

#define ts3CaseReturn( pCaseValue, pReturnValue ) \
	case pCaseValue: return pReturnValue

#define ts3CaseReturnStr( pCaseValue ) \
	case pCaseValue: return #pCaseValue

#define for_index( pIndexName, pInitVal, pLessThan ) \
	for( size_t pIndexName = pInitVal; pIndexName < pLessThan; ++pIndexName )

#define for_index_type( pIndexType, pIndexName, pInitVal, pLessThan ) \
	for( pIndexType pIndexName = pInitVal; pIndexName < pLessThan; ++pIndexName )

namespace ts3
{

	template <typename TEnum>
	class EnumTypeInfo;

	template <typename TValue, typename TAllocator = std::allocator<TValue>>
	using Array = std::vector<TValue, TAllocator>;

	template <typename TTarget, typename TSrc>
	TS3_ATTR_NO_DISCARD inline TTarget * dynamic_cast_check( TSrc * pSource )
	{
		if( auto * targetPtr = dynamic_cast<TTarget *>( pSource ) )
		{
			return targetPtr;
		}
		TS3_PCL_DEBUG_BREAK();
		return nullptr;
	}

	template <typename TTarget, typename TSrc>
	TS3_ATTR_NO_DISCARD inline TTarget * dynamic_cast_throw( TSrc * pSource )
	{
		if( auto * targetPtr = dynamic_cast<TTarget *>( pSource ) )
		{
			return targetPtr;
		}
		throw std::bad_cast();
	}

	template <typename TTarget, typename TSrc>
	TS3_ATTR_NO_DISCARD inline std::shared_ptr<TTarget> dynamic_ptr_cast_check( std::shared_ptr<TSrc> pSource )
	{
		if( auto targetPtr = std::dynamic_pointer_cast<TTarget>( std::move( pSource ) ) )
		{
			return targetPtr;
		}
		TS3_PCL_DEBUG_BREAK();
		return nullptr;
	}

	template <typename TTarget, typename TSrc>
	TS3_ATTR_NO_DISCARD inline std::shared_ptr<TTarget> dynamic_ptr_cast_throw( std::shared_ptr<TSrc> pSource )
	{
		if( auto targetPtr = std::dynamic_pointer_cast<TTarget>( std::move( pSource ) ) )
		{
			return targetPtr;
		}
		throw std::bad_cast();
	}

}

#endif // __TS3_CORE_CORE_DEFS_H__
