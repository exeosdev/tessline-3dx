
#pragma once

#ifndef __TS3_ENGINE_RES_FTD_FREETYPE_FONT_COMMON_H__
#define __TS3_ENGINE_RES_FTD_FREETYPE_FONT_COMMON_H__

#include "../fontMetrics.h"

namespace ts3
{

	class FreeTypeFontFace;
	class FreeTypeFontObject;

	using FreeTypeFontFacePtr = std::unique_ptr<FreeTypeFontFace>;
	using FreeTypeFontObjectPtr = std::unique_ptr<FreeTypeFontObject>;

} // namespace ts3

#endif // __TS3_ENGINE_RES_FTD_FREETYPE_FONT_COMMON_H__
