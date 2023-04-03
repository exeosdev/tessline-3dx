
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

#define ts3CaseDefaultBreak() \
	default: break

#define ts3CaseDefaultReturn( pReturnValue ) \
	default: return pReturnValue

namespace ts3
{

	template <typename TEnum>
	class EnumTypeInfo;

	template <typename TValue, typename TAllocator = std::allocator<TValue>>
	using Array = std::vector<TValue, TAllocator>;

	template <typename TResult, typename TSrc>
	TS3_ATTR_NO_DISCARD inline TResult dynamic_cast_dbg( TSrc * pSource )
	{
	#if( TS3_DEBUG )
		auto * targetPtr = dynamic_cast<TResult>( pSource );
		ts3DebugAssert( targetPtr );
	#else
		auto * targetPtr = static_cast<TResult>( pSource );
	#endif
		return targetPtr;
	}

	template <typename TResult, typename TSrc>
	TS3_ATTR_NO_DISCARD inline TResult dynamic_cast_check( TSrc * pSource )
	{
		if( auto * targetPtr = dynamic_cast<TResult>( pSource ) )
		{
			return targetPtr;
		}
		TS3_PCL_DEBUG_BREAK();
		return nullptr;
	}

	template <typename TResult, typename TSrc>
	TS3_ATTR_NO_DISCARD inline TResult dynamic_cast_throw( TSrc * pSource )
	{
		if( auto * targetPtr = dynamic_cast<TResult>( pSource ) )
		{
			return targetPtr;
		}
		throw std::bad_cast();
	}

	template <typename TResult, typename TSrc>
	TS3_ATTR_NO_DISCARD inline std::shared_ptr<TResult> dynamic_ptr_cast_check( std::shared_ptr<TSrc> pSource )
	{
		if( auto targetPtr = std::dynamic_pointer_cast<TResult>( std::move( pSource ) ) )
		{
			return targetPtr;
		}
		TS3_PCL_DEBUG_BREAK();
		return nullptr;
	}

	template <typename TResult, typename TSrc>
	TS3_ATTR_NO_DISCARD inline std::shared_ptr<TResult> dynamic_ptr_cast_throw( std::shared_ptr<TSrc> pSource )
	{
		if( auto targetPtr = std::dynamic_pointer_cast<TResult>( std::move( pSource ) ) )
		{
			return targetPtr;
		}
		throw std::bad_cast();
	}

}

#endif // __TS3_CORE_CORE_DEFS_H__
