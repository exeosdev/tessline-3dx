
#ifndef __TS3_STDEXT_RANGE_H__
#define __TS3_STDEXT_RANGE_H__

#include "staticLimits.h"

namespace ts3
{

	enum class RangeType
	{
		Exclusive,
		Inclusive,
		InclusiveLeft,
		InclusiveRight
	};

	template <typename Tp, RangeType tpRangeType = RangeType::Inclusive>
	struct Range
	{
		Tp begin;
		Tp end;
	};

	template <typename Tp>
	using ExclusiveRange = Range<Tp, RangeType::Exclusive>;

	template <typename Tp>
	using InclusiveRange = Range<Tp, RangeType::Inclusive>;

	template <typename Tp, RangeType>
	struct LeftRangeBound;

	template <typename Tp, RangeType>
	struct RightRangeBound;

	template <typename Tp, RangeType tpRangeType>
	inline bool checkRangeLeftBound( const Range<Tp, tpRangeType> & pRange, Tp pValue )
	{
		return LeftRangeBound<Tp, tpRangeType>::inside( pRange, pValue );
	}

	template <typename Tp, RangeType tpRangeType>
	inline bool checkRangeRightBound( const Range<Tp, tpRangeType> & pRange, Tp pValue )
	{
		return RightRangeBound<Tp, tpRangeType>::inside( pRange, pValue );
	}

	template <typename Tp, RangeType tpRangeType>
	inline bool checkValueInsideRange( const Range<Tp, tpRangeType> & pRange, Tp pValue )
	{
		return checkRangeLeftBound( pRange, pValue ) && checkRangeRightBound( pRange, pValue );
	}

	template <typename Tp, RangeType tpRangeType>
	inline bool checkValueOutsideRange( const Range<Tp, tpRangeType> & pRange, Tp pValue )
	{
		return !checkRangeLeftBound( pRange, pValue ) || !checkRangeRightBound( pRange, pValue );
	}

	template <typename Tp>
	inline bool checkRangeOverlap( const InclusiveRange<Tp> & pFirst, const InclusiveRange<Tp> & pSecond )
	{
		return checkValueInsideRange( pFirst, pSecond.begin ) || checkValueInsideRange( pFirst, pSecond.end );
	}

	template <typename Tp>
	inline bool checkRangeSubrange( const InclusiveRange<Tp> & pRange, const InclusiveRange<Tp> & pSubrange )
	{
		return checkRangeLeftBound( pRange, pSubrange.begin ) && checkRangeRightBound( pRange, pSubrange.end );
	}

	template <typename Tp, RangeType tpRangeType>
	struct LeftRangeBound
	{
		static bool inside( const Range<Tp, tpRangeType> & pRange, Tp pValue )
		{
			return pValue > pRange.begin;
		}
	};

	template <typename Tp>
	struct LeftRangeBound<Tp, RangeType::Inclusive>
	{
		static bool inside( const Range<Tp, RangeType::Inclusive> & pRange, Tp pValue )
		{
			return pValue >= pRange.begin;
		}
	};

	template <typename Tp>
	struct LeftRangeBound<Tp, RangeType::InclusiveLeft>
	{
		static bool inside( const Range<Tp, RangeType::InclusiveLeft> & pRange, Tp pValue )
		{
			return pValue >= pRange.begin;
		}
	};

	template <typename Tp, RangeType tpRangeType>
	struct RightRangeBound
	{
		static bool inside( const Range<Tp, tpRangeType> & pRange, Tp pValue )
		{
			return pValue < pRange.end;
		}
	};

	template <typename Tp>
	struct RightRangeBound<Tp, RangeType::Inclusive>
	{
		static bool inside( const Range<Tp, RangeType::Inclusive> & pRange, Tp pValue )
		{
			return pValue <= pRange.end;
		}
	};

	template <typename Tp>
	struct RightRangeBound<Tp, RangeType::InclusiveRight>
	{
		static bool inside( const Range<Tp, RangeType::InclusiveRight> & pRange, Tp pValue )
		{
			return pValue <= pRange.end;
		}
	};

}

#endif // __TS3_STDEXT_RANGE_H__
