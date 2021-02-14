
#ifndef __TS3EXT_RCSUPPORT_FREE_TYPE_COMMON_H__
#define __TS3EXT_RCSUPPORT_FREE_TYPE_COMMON_H__

#include <ts3/engine/rcs/font/fontCommon.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftglyph.h>

namespace ts3
{

	class FreeTypeFontFace;
	class FreeTypeFontObject;

	using FreeTypeFontDataBuffer = DynamicMemoryBuffer;
	using FreeTypeFontFacePtr = std::unique_ptr<FreeTypeFontFace>;
	using FreeTypeFontObjectPtr = std::unique_ptr<FreeTypeFontObject>;

	struct FTGlyphData
	{
		FT_Glyph ftGlyph = nullptr;
		FT_Glyph_Metrics ftGlyphMetrics;
	};

	struct FTGlyphImage
	{
		FT_BitmapGlyph ftGlyphBitmap = nullptr;
	};

	FreeTypeFontObjectPtr createFreeTypeFontObjectFromMemory( const void * pData, size_t pDataSize );

}

#endif // __TS3EXT_RCSUPPORT_FREE_TYPE_COMMON_H__
