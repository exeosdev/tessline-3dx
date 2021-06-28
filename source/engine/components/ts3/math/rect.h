
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

	template <typename TpOffset, typename TpSize>
	struct Rect
	{
	public:
	    union
        {
	        struct
            {
                Vector2<TpOffset> offset;
                Vector2<TpSize> size;
            };

            struct
            {
                TpOffset x;
                TpOffset y;
                TpSize width;
                TpSize height;
            };
	    };

	public:
        constexpr Rect() = default;

        constexpr Rect( const Rect & ) = default;
		constexpr Rect & operator=( const Rect & ) = default;

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr explicit Rect( TpScalar pScalar ) noexcept
		: offset( pScalar, pScalar )
		, size( pScalar, pScalar )
		{}

		template <typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		constexpr Rect( TpScalar pWidth, TpScalar pHeight ) noexcept
		: offset( 0, 0 )
		, size( pWidth, pHeight )
		{}

		template <typename TpX, typename TpY, typename TpWidth, typename TpHeight>
		constexpr Rect( TpX pX, TpY pY, TpWidth pWidth, TpHeight pHeight = 0 ) noexcept
		: offset( pX, pY )
		, size( pWidth, ( pHeight == 0 ) ? pWidth : pHeight )
		{}

		template <typename Tp1, typename Tp2>
		constexpr Rect( const Vector2<Tp1> & pOffset, const Vector2<Tp2> & pSize ) noexcept
		: offset( pOffset )
		, size( pSize )
		{}

        TS3_PCL_ATTR_NO_DISCARD TpSize area() const
        {
            return width * height;
        }

        TS3_PCL_ATTR_NO_DISCARD bool isNonZero() const
        {
            return ( width != 0 ) && ( height != 0 );
        }

        TS3_PCL_ATTR_NO_DISCARD bool isZero() const
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
