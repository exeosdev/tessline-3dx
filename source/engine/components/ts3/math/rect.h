
#ifndef __TS3_MATH_RECT_H__
#define __TS3_MATH_RECT_H__

#include "vector.h"

namespace ts3::math
{

	template <typename TpOffset, typename TpDimensions>
	struct Rect
	{
	public:
		union
		{
			struct
			{
				Vector2<TpOffset> baseOffset;
				Vector2<TpDimensions> dimensions;
			};
			struct
			{
				TpOffset x;
				TpOffset y;
				TpDimensions width;
				TpDimensions height;
			};
		};

	public:
		constexpr Rect( const Rect & ) = default;
		constexpr Rect & operator=( const Rect & ) = default;

		constexpr Rect() = default;

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Rect( TpScalar pScalar ) noexcept
		: x( static_cast<TpOffset>( pScalar ) )
		, y( static_cast<TpOffset>( pScalar ) )
		, width( static_cast<TpDimensions>( pScalar ) )
		, height( static_cast<TpDimensions>( pScalar ) )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr Rect( TpScalar pWidth, TpScalar pHeight = 0 ) noexcept
		: x( static_cast<TpOffset>( 0 ) )
		, y( static_cast<TpOffset>( 0 ) )
		, width( static_cast<TpDimensions>( pWidth ) )
		, height( static_cast<TpDimensions>( ( pHeight == 0 ) ? pWidth : pHeight ) )
		{}

		template <typename TpX, typename TpY, typename TpWidth, typename TpHeight>
		constexpr Rect( TpX pX, TpY pY, TpWidth pWidth, TpHeight pHeight ) noexcept
		: x( static_cast<TpOffset>( pX ) )
		, y( static_cast<TpOffset>( pY ) )
		, width( static_cast<TpDimensions>( pWidth ) )
		, height( static_cast<TpDimensions>( pHeight ) )
		{}

		template <typename TpOff, typename TpDim>
		constexpr Rect( const Vector2<TpOff> & pOffset, const Vector2<TpDim> & pDimensions ) noexcept
		: baseOffset( pOffset )
		, dimensions( pDimensions )
		{}
	};

	using Recti32 = Rect<int32, uint32>;
	using Rectu32 = Rect<uint32, uint32>;
	using Recti64 = Rect<int64, uint64>;
	using Rectu64 = Rect<uint64, uint64>;
	using Rectf   = Rect<float, float>;

}

#endif // __TS3_MATH_VECTOR_OPS_H__
