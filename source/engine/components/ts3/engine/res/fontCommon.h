
#pragma once

#ifndef __TS3_ENGINE_RES_FONT_COMMON_H__
#define __TS3_ENGINE_RES_FONT_COMMON_H__

#include "../prerequisites.h"

namespace ts3
{

	class Font;

	using char_code_point_t = uint32;
	using font_size_t = uint32;
	using font_type_t = uint64;

	enum EFontBaseType : uint32
    {
	    Unknown,
	    DynamicFont,
	    StaticFont
    };

}

#endif // __TS3_ENGINE_RES_FONT_COMMON_H__
