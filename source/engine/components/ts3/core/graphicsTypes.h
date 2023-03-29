
#pragma once

#ifndef __TS3_CORE_GRAPHICS_TYPES_H__
#define __TS3_CORE_GRAPHICS_TYPES_H__

#include "prerequisites.h"

namespace ts3
{

	namespace cxdefs
	{

		inline constexpr uint32 declarePixelDataLayout( uint8 pIndex, uint8 pChannelsNum )
		{
			return ( ( ( uint32 )pChannelsNum ) << 8 ) | ( uint32 )pIndex;
		}

	}

	enum class EPixelDataLayout : uint32
	{
		Undefined = 0,
		Alpha     = cxdefs::declarePixelDataLayout( 0x1, 1 ),
		Depth     = cxdefs::declarePixelDataLayout( 0x2, 1 ),
		DS        = cxdefs::declarePixelDataLayout( 0x3, 2 ),
		BGR       = cxdefs::declarePixelDataLayout( 0x4, 3 ),
		BGRA      = cxdefs::declarePixelDataLayout( 0x5, 4 ),
		Red       = cxdefs::declarePixelDataLayout( 0x6, 1 ),
		RG        = cxdefs::declarePixelDataLayout( 0x7, 2 ),
		RGB       = cxdefs::declarePixelDataLayout( 0x8, 3 ),
		RGBA      = cxdefs::declarePixelDataLayout( 0x9, 4 ),
		S3TC      = cxdefs::declarePixelDataLayout( 0xA, 1 ),
	};

	namespace cxdefs
	{

		inline constexpr uint32 getPixelDataLayoutChannelsNum( EPixelDataLayout pPixelDataLayout )
		{
			return ( ( uint32 )pPixelDataLayout >> 8 ) & 0xFF;
		}

	}

} // namespace ts3

#endif // __TS3_CORE_GRAPHICS_TYPES_H__
