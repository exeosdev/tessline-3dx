
#ifndef __TS3_MATH_RECT_H__
#define __TS3_MATH_RECT_H__

#include "vector.h"

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#  pragma clang diagnostic ignored "-Wnested-anon-types"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpedantic"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( push )
#  pragma warning( disable: 4201 )  // 'Nonstandard extension used: nameless struct/union'
#endif

namespace ts3::math
{

	template <typename TOffset, typename TSize>
	struct Rect
	{
	public:
		union
		{
			struct
			{
				Vector2<TOffset> offset;
				Vector2<TSize> size;
			};

			struct
			{
				TOffset x;
				TOffset y;
				TSize width;
				TSize height;
			};
		};

	public:
		constexpr Rect() = default;

		constexpr Rect( const Rect & ) = default;
		constexpr Rect & operator=( const Rect & ) = default;

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr explicit Rect( TScalar pScalar ) noexcept
		: offset( pScalar, pScalar )
		, size( pScalar, pScalar )
		{}

		template <typename TScalar, enable_if_scalar_t<TScalar> = true>
		constexpr Rect( TScalar pWidth, TScalar pHeight ) noexcept
		: offset( 0, 0 )
		, size( pWidth, pHeight )
		{}

		template <typename TX, typename TY, typename TWidth, typename THeight>
		constexpr Rect( TX pX, TY pY, TWidth pWidth, THeight pHeight = 0 ) noexcept
		: offset( pX, pY )
		, size( pWidth, ( pHeight == 0 ) ? pWidth : pHeight )
		{}

		template <typename T1, typename T2>
		constexpr Rect( const Vector2<T1> & pOffset, const Vector2<T2> & pSize ) noexcept
		: offset( pOffset )
		, size( pSize )
		{}

		TS3_PCL_ATTR_FUNC_NO_DISCARD TSize area() const
		{
			return width * height;
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD bool isNonZero() const
		{
			return ( width != 0 ) && ( height != 0 );
		}

		TS3_PCL_ATTR_FUNC_NO_DISCARD bool isZero() const
		{
			return ( width == 0 ) || ( height == 0 );
		}
	};

	using Recti32 = Rect<int32, uint32>;
	using Rectu32 = Rect<uint32, uint32>;
	using Recti64 = Rect<int64, uint64>;
	using Rectu64 = Rect<uint64, uint64>;
	using Rectf   = Rect<float, float>;

}

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __TS3_MATH_RECT_H__
