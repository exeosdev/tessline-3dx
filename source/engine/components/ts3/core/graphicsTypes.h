
#pragma once

#ifndef __TS3_CORE_CORE_GRAPHICS_TYPES_H__
#define __TS3_CORE_CORE_GRAPHICS_TYPES_H__

#include "prerequisites.h"

namespace ts3
{

	inline constexpr uint32 ecDeclarePixelDataLayout( uint8 pIndex, uint8 pChannelsNum )
	{
		return ( ( ( uint32 )pChannelsNum ) << 8 ) | ( uint32 )pIndex;
	}

	enum class EPixelDataLayout : uint32
	{
		Alpha        = ecDeclarePixelDataLayout( 0x1, 1 ),
		Depth        = ecDeclarePixelDataLayout( 0x2, 1 ),
		DepthStencil = ecDeclarePixelDataLayout( 0x3, 2 ),
		BGR          = ecDeclarePixelDataLayout( 0x4, 3 ),
		BGRA         = ecDeclarePixelDataLayout( 0x5, 4 ),
		Red          = ecDeclarePixelDataLayout( 0x6, 1 ),
		RG           = ecDeclarePixelDataLayout( 0x7, 2 ),
		RGB          = ecDeclarePixelDataLayout( 0x8, 3 ),
		RGBA         = ecDeclarePixelDataLayout( 0x9, 4 ),
		S3TC         = ecDeclarePixelDataLayout( 0xA, 1 ),
		Undefined    = 0,
	};

	inline constexpr uint32 ecGetPixelDataLayoutChannelsNum( EPixelDataLayout pPixelDataLayout )
	{
		return ( ( uint32 )pPixelDataLayout >> 8 ) & 0xFF;
	}

}

#endif // __TS3_CORE_CORE_GRAPHICS_TYPES_H__
