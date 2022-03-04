
#ifndef __TS3_STDEXT_STATIC_ALGO_H__
#define __TS3_STDEXT_STATIC_ALGO_H__

#include "prerequisites.h"
#include <numeric>

namespace ts3
{

	template <typename Tp1, typename Tp2 = Tp1>
	struct CmpEqual
	{
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs == pRhs;
		}
	};

	template <typename Tp1>
	struct CmpEqualT
	{
		template <typename Tp2>
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs == pRhs;
		}
	};

	template <typename Tp1, typename Tp2 = Tp1>
	struct CmpNotEqual
	{
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs != pRhs;
		}
	};

	template <typename Tp1>
	struct CmpNotEqualT
	{
		template <typename Tp2>
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs != pRhs;
		}
	};

	template <typename Tp1, typename Tp2 = Tp1>
	struct CmpLess
	{
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs < pRhs;
		}
	};

	template <typename Tp1>
	struct CmpLessT
	{
		template <typename Tp2>
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs < pRhs;
		}
	};

	template <typename Tp1, typename Tp2 = Tp1>
	struct CmpLessEqual
	{
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs <= pRhs;
		}
	};

	template <typename Tp1>
	struct CmpLessEqualT
	{
		template <typename Tp2>
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs <= pRhs;
		}
	};

	template <typename Tp1, typename Tp2 = Tp1>
	struct CmpGreater
	{
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs > pRhs;
		}
	};

	template <typename Tp1>
	struct CmpGreaterT
	{
		template <typename Tp2>
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs > pRhs;
		}
	};

	template <typename Tp1, typename Tp2 = Tp1>
	struct CmpGreaterEqual
	{
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs >= pRhs;
		}
	};

	template <typename Tp1>
	struct CmpGreaterEqualT
	{
		template <typename Tp2>
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs >= pRhs;
		}
	};

	template <intmax_t tValue>
	struct StaticAbs
	{
		static constexpr intmax_t value = ( tValue < 0 ) ? -tValue : tValue;
	};

	template <intmax_t tValue>
	struct StaticSignOf
	{
		static constexpr int32_t value = ( tValue < 0 ) ? -1 : 1;
	};

	template <intmax_t tVal1, intmax_t tVal2, intmax_t... tValN>
	struct StaticMax
	{
		static constexpr intmax_t value = StaticMax<tVal1, StaticMax<tVal2, tValN...>::value>::value;
	};

	template <intmax_t tVal1, intmax_t tVal2>
	struct StaticMax<tVal1, tVal2>
	{
		static constexpr intmax_t value = tVal1 >= tVal2 ? tVal1 : tVal2;
	};

	template <intmax_t tVal1, intmax_t tVal2, intmax_t... tValN>
	struct StaticMin
	{
		static constexpr intmax_t value = StaticMin<StaticMin<tVal1, tVal2>::value, tValN...>::value;
	};

	template <intmax_t tVal1, intmax_t tVal2>
	struct StaticMin<tVal1, tVal2>
	{
		static constexpr intmax_t value = tVal1 <= tVal2 ? tVal1 : tVal2;
	};

	template <typename... _Types>
	struct StaticMaxSizeofTp
	{
		static constexpr size_t value = static_cast< size_t >( StaticMax<sizeof( _Types )...>::value );
	};

	template <intmax_t... tValues>
	struct StaticMaxSizeofVal
	{
		static constexpr size_t value = StaticMaxSizeofTp<typename IntTypeByValue<tValues>::Type...>::value;
	};

	template <typename... _Types>
	struct StaticMaxAlignofTp
	{
		static constexpr size_t value = static_cast< size_t >( StaticMax<alignof( _Types )...>::value );
	};

	template <intmax_t... tValues>
	struct StaticMaxAlignofVal
	{
		static constexpr size_t value = StaticMaxAlignofTp<typename IntTypeByValue<tValues>::Type...>::value;
	};

}

#endif /* __TS3_STDEXT_STATIC_ALGO_H__ */
