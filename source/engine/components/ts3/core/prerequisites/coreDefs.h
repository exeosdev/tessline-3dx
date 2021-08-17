
#pragma once

#ifndef __TS3_CORE_CORE_DEFS_H__
#define __TS3_CORE_CORE_DEFS_H__

#include <memory>

namespace ts3
{

	template <typename TpTarget, typename TpSrc>
	inline TpTarget * dynamic_cast_check( TpSrc * pSource )
	{
		if( auto * targetPtr = dynamic_cast<TpTarget *>( pSource ) )
		{
			return targetPtr;
		}
		TS3_PCL_DEBUG_BREAK();
		return nullptr;
	}

	template <typename TpTarget, typename TpSrc>
	inline TpTarget * dynamic_cast_throw( TpSrc * pSource )
	{
		if( auto * targetPtr = dynamic_cast<TpTarget *>( pSource ) )
		{
			return targetPtr;
		}
		throw std::bad_cast();
	}

	template <typename TpTarget, typename TpSrc>
	inline std::shared_ptr<TpTarget> dynamic_shared_ptr_cast_check( std::shared_ptr<TpSrc> pSource )
	{
		if( auto targetPtr = std::dynamic_pointer_cast<TpTarget>( std::move( pSource ) ) )
		{
			return targetPtr;
		}
		TS3_PCL_DEBUG_BREAK();
		return nullptr;
	}

	template <typename TpTarget, typename TpSrc>
	inline std::shared_ptr<TpTarget> dynamic_shared_ptr_cast_throw( std::shared_ptr<TpSrc> pSource )
	{
		if( auto targetPtr = std::dynamic_pointer_cast<TpTarget>( std::move( pSource ) ) )
		{
			return targetPtr;
		}
		throw std::bad_cast();
	}

	template <typename TpResult, typename TpInput>
	inline TpResult trunc_numeric_cast( TpInput pInput )
	{
		ts3DebugAssert( pInput <= Limits<TpResult>::maxValue );
		return static_cast<TpResult>( pInput );
	}

}

#endif // __TS3_CORE_CORE_DEFS_H__
