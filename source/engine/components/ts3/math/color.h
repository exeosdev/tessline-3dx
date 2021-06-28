
#ifndef __TS3_MATH_COLOR_H__
#define __TS3_MATH_COLOR_H__

#include "prerequisites.h"
#include <ts3/stdext/staticLimits.h>

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

	union RGBAColor
	{
	public:
		struct
		{
			uint8 u8Red;
			uint8 u8Green;
			uint8 u8Blue;
			uint8 u8Alpha;
		};
		uint32 u32Code = 0xFF;

	public:
		constexpr RGBAColor() noexcept
		: u8Red( 0u )
		, u8Green( 0u )
		, u8Blue( 0u )
		, u8Alpha( 255u )
		{}

		constexpr RGBAColor( uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pAlpha = 0xFF ) noexcept
		: u8Red( pRed )
		, u8Green( pGreen )
		, u8Blue( pBlue )
		, u8Alpha( pAlpha )
		{}
	};

	template <typename TpReal>
	union RGBAColorNorm
	{
	public:
		using ValueType = TpReal;

		struct
		{
			TpReal fpRed;
			TpReal fpGreen;
			TpReal fpBlue;
			TpReal fpAlpha;
		};
		TpReal rgbaArray[4];

	public:
		constexpr RGBAColorNorm() noexcept
		: fpRed( static_cast<TpReal>( 0 ) )
		, fpGreen( static_cast<TpReal>( 0 ) )
		, fpBlue( static_cast<TpReal>( 0 ) )
		, fpAlpha( static_cast<TpReal>( 1 ) )
		{}

		constexpr RGBAColorNorm( TpReal pRed, TpReal pGreen, TpReal pBlue, TpReal pAlpha = static_cast<TpReal>( 1 ) ) noexcept
		: fpRed( pRed )
		, fpGreen( pGreen )
		, fpBlue( pBlue )
		, fpAlpha( pAlpha )
		{}

		constexpr RGBAColorNorm( RGBAColor pColor )
		: fpRed( static_cast<TpReal>( pColor.u8Red ) / static_cast<TpReal>( ts3::Limits<decltype( pColor.u8Red )>::maxValue ) )
		, fpGreen( static_cast<TpReal>( pColor.u8Green ) / static_cast<TpReal>( ts3::Limits<decltype( pColor.u8Red )>::maxValue ) )
		, fpBlue( static_cast<TpReal>( pColor.u8Blue ) / static_cast<TpReal>( ts3::Limits<decltype( pColor.u8Red )>::maxValue ) )
		, fpAlpha( static_cast<TpReal>( pColor.u8Alpha ) / static_cast<TpReal>( ts3::Limits<decltype( pColor.u8Red )>::maxValue ) )
		{}
	};

	using RGBAColorU8 = RGBAColor;
	using RGBAColorR32Norm = RGBAColorNorm<float>;
	using RGBAColorR64Norm = RGBAColorNorm<double>;

	inline bool operator==( const RGBAColor & pLhs, const RGBAColor & pRhs )
	{
		return pLhs.u32Code == pRhs.u32Code;
	}

	inline bool operator!=( const RGBAColor & pLhs, const RGBAColor & pRhs )
	{
		return pLhs.u32Code != pRhs.u32Code;
	}

	template <typename TpReal>
	inline bool operator==( const RGBAColorNorm<TpReal> & pLhs, const RGBAColorNorm<TpReal> & pRhs )
	{
		return ( pLhs.fpRed == pRhs.fpRed ) && ( pLhs.fpGreen == pRhs.fpGreen ) && ( pLhs.fpBlue == pRhs.fpBlue ) && ( pLhs.fpAlpha == pRhs.fpAlpha );
	}

	template <typename TpReal>
	inline bool operator!=( const RGBAColorNorm<TpReal> & pLhs, const RGBAColorNorm<TpReal> & pRhs )
	{
		return ( pLhs.fpRed != pRhs.fpRed ) || ( pLhs.fpGreen != pRhs.fpGreen ) || ( pLhs.fpBlue != pRhs.fpBlue ) || ( pLhs.fpAlpha != pRhs.fpAlpha );
	}

	RGBAColorU8 generateRandomColor();

	namespace colors
	{

		constexpr RGBAColorU8 cxColorBlueDeepSky{ 0x00, 0xB2, 0xEE };
		constexpr RGBAColorU8 cxColorBlueFacebookDark{ 0x3b, 0x59, 0x98 };
		constexpr RGBAColorU8 cxColorBlueFacebookLight{ 0x8b, 0x9d, 0xc3 };
		constexpr RGBAColorU8 cxColorBlueNavyDark{ 0x00, 0x00, 0x80 };
		constexpr RGBAColorU8 cxColorBlueSignatory{ 0x00, 0x3F, 0x77 };
		constexpr RGBAColorU8 cxColorPeachPuff{ 0xFF, 0xDA, 0xB9 };
		constexpr RGBAColorU8 cxColorTeal{ 0x00, 0x80, 0x80 };
		constexpr RGBAColorU8 cxColorTopaz{ 0x01, 0x98, 0xE1 };

	}

}

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __TS3_MATH_COLOR_H__
