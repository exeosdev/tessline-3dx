
#ifndef __TS3_STDEXT_RANGE_H__
#define __TS3_STDEXT_RANGE_H__

#include "staticLimits.h"
#include "utilities.h"

namespace ts3
{

	enum class ERangeType
	{
		Exclusive,
		Inclusive,
		InclusiveLeft,
		InclusiveRight
	};

	template <typename TVal, ERangeType tERangeType = ERangeType::Inclusive>
	struct Range
	{
		TVal begin;
		TVal end;
	};

	template <typename TVal>
	using ExclusiveRange = Range<TVal, ERangeType::Exclusive>;

	template <typename TVal>
	using InclusiveRange = Range<TVal, ERangeType::Inclusive>;

	template <typename TVal, ERangeType>
	struct LeftRangeBound;

	template <typename TVal, ERangeType>
	struct RightRangeBound;

	template <ERangeType tERangeType, typename TVal>
	inline bool checkRangeLeftBound( const Range<TVal, tERangeType> & pRange, TVal pValue )
	{
		return LeftRangeBound<TVal, tERangeType>::inside( pRange, pValue );
	}

	template <ERangeType tERangeType, typename TVal>
	inline bool checkRangeRightBound( const Range<TVal, tERangeType> & pRange, TVal pValue )
	{
		return RightRangeBound<TVal, tERangeType>::inside( pRange, pValue );
	}

	template <ERangeType tERangeType, typename TVal>
	inline bool checkValueInsideRange( const Range<TVal, tERangeType> & pRange, TVal pValue )
	{
		return checkRangeLeftBound( pRange, pValue ) && checkRangeRightBound( pRange, pValue );
	}

	template <typename TVal>
	inline bool checkValueInsideInclusiveRange( const Range<TVal, ERangeType::Inclusive> & pRange, TVal pValue )
	{
		return checkRangeLeftBound( pRange, pValue ) && checkRangeRightBound( pRange, pValue );
	}

	template <ERangeType tERangeType, typename TVal>
	inline bool checkValueOutsideRange( const Range<TVal, tERangeType> & pRange, TVal pValue )
	{
		return !checkRangeLeftBound( pRange, pValue ) || !checkRangeRightBound( pRange, pValue );
	}

	template <typename TVal>
	inline bool checkRangeOverlap( const InclusiveRange<TVal> & pFirst, const InclusiveRange<TVal> & pSecond )
	{
		return checkValueInsideRange( pFirst, pSecond.begin ) || checkValueInsideRange( pFirst, pSecond.end );
	}

	template <typename TVal>
	inline bool checkRangeSubRange( const InclusiveRange<TVal> & pRange, const InclusiveRange<TVal> & pSubRange )
	{
		return checkRangeLeftBound( pRange, pSubRange.begin ) && checkRangeRightBound( pRange, pSubRange.end );
	}

	template <typename TVal, ERangeType tERangeType>
	struct LeftRangeBound
	{
		static bool inside( const Range<TVal, tERangeType> & pRange, TVal pValue )
		{
			return pValue > pRange.begin;
		}
	};

	template <typename TVal>
	struct LeftRangeBound<TVal, ERangeType::Inclusive>
	{
		static bool inside( const Range<TVal, ERangeType::Inclusive> & pRange, TVal pValue )
		{
			return pValue >= pRange.begin;
		}
	};

	template <typename TVal>
	struct LeftRangeBound<TVal, ERangeType::InclusiveLeft>
	{
		static bool inside( const Range<TVal, ERangeType::InclusiveLeft> & pRange, TVal pValue )
		{
			return pValue >= pRange.begin;
		}
	};

	template <typename TVal, ERangeType tERangeType>
	struct RightRangeBound
	{
		static bool inside( const Range<TVal, tERangeType> & pRange, TVal pValue )
		{
			return pValue < pRange.end;
		}
	};

	template <typename TVal>
	struct RightRangeBound<TVal, ERangeType::Inclusive>
	{
		static bool inside( const Range<TVal, ERangeType::Inclusive> & pRange, TVal pValue )
		{
			return pValue <= pRange.end;
		}
	};

	template <typename TVal>
	struct RightRangeBound<TVal, ERangeType::InclusiveRight>
	{
		static bool inside( const Range<TVal, ERangeType::InclusiveRight> & pRange, TVal pValue )
		{
			return pValue <= pRange.end;
		}
	};

	template <typename TSize, typename TOffset = TSize>
	struct Region
	{
		static_assert( !std::is_signed_v<TSize>, "Region size cannot be negative, hence usage of signed types is forbidden." );

		using Range = InclusiveRange<TSize>;

		TOffset offset = 0;
		TSize size = 0;

		TS3_PCL_ATTR_FUNC_NO_DISCARD explicit operator bool() const
		{
			return !empty();
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD Range asRange() const
		{
			return { offset, offset + size };
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD bool empty() const
		{
			return size == 0;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD bool contains( const Region<TSize, TOffset> & pOther ) const
		{
			return checkRangeSubRange( asRange(), pOther.asRange() );
		}

		void reset()
		{
			offset = 0;
			size = 0;
		}
	};

	template <typename TSize, typename TOffset>
	inline bool operator==( const Region<TSize, TOffset> & pLhs, const Region<TSize, TOffset> & pRhs )
	{
		return ( pLhs.offset == pRhs.offset ) && ( pLhs.size == pRhs.size );
	}

	template <typename TSize, typename TOffset>
	inline bool operator!=( const Region<TSize, TOffset> & pLhs, const Region<TSize, TOffset> & pRhs )
	{
		return ( pLhs.offset != pRhs.offset ) || ( pLhs.size != pRhs.size );
	}

	template <typename TOffset, typename TSize, typename TLimit>
	inline Region<TSize, TOffset> getValidRegion( TOffset pOffset, TSize pSize, TLimit pSizeLimit )
	{
		const auto validOffset = getMinOf( pOffset, pSizeLimit );
		const auto maxRegionSize = pSizeLimit - validOffset;
		const auto regionSize = getMinOf( pSize, maxRegionSize );

		return { numeric_cast<TOffset>( validOffset ), numeric_cast<TSize>( regionSize ) };
	}

	template <typename TOffset, typename TSize, typename TLimit>
	inline Region<TSize, TOffset> getValidRegion( const Region<TSize, TOffset> & pRegion, TLimit pSizeLimit )
	{
		getValidRegion( pRegion.offset, pRegion.size, pSizeLimit );
	}

}

#endif // __TS3_STDEXT_RANGE_H__
