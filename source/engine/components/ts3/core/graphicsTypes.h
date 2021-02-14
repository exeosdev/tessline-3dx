
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
		ABGR = ecDeclarePixelDataLayout( 1, 4 ),
		BGR  = ecDeclarePixelDataLayout( 3, 3 ),
		BGRA = ecDeclarePixelDataLayout( 4, 4 ),
		RGB  = ecDeclarePixelDataLayout( 5, 3 ),
		RGBA = ecDeclarePixelDataLayout( 6, 4 ),
		GS   = ecDeclarePixelDataLayout( 7, 1 ),
		GSA  = ecDeclarePixelDataLayout( 8, 2 ),
		S3TC = ecDeclarePixelDataLayout( 7, 1 ),
		Undefined = 0,
	};

	inline constexpr uint32 ecGetPixelDataLayoutChannelsNum( EPixelDataLayout pPixelDataLayout )
	{
		return ( ( uint32 )pPixelDataLayout >> 8 ) & 0xFF;
	}

}

#endif // __TS3_CORE_CORE_GRAPHICS_TYPES_H__
