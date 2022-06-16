
#pragma once

#ifndef __TS3_ENGINE_RES_FTD_FREETYPE_FONT_OBJECT_H__
#define __TS3_ENGINE_RES_FTD_FREETYPE_FONT_OBJECT_H__

#include "ftdFreeTypeCommon.h"
#include <ts3/stdext/byteArray.h>
#include <unordered_map>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftglyph.h>

namespace ts3
{

	class FreeTypeFontObject
	{
	public:
		FT_Library const mFTLibrary;

		FreeTypeFontObject( std::nullptr_t );
		FreeTypeFontObject( FT_Library pFTLibrary, DynamicByteArray pFontData );
		~FreeTypeFontObject();

		explicit operator bool() const
		{
			return _ftLibraryInstance != nullptr;
		}

		FreeTypeFontFace * loadFace( uint32 pFontSize );

		bool setActiveFace( uint32 pFontSize, const math::Vec2u32 & pFontResolution );

		bool loadGlyph( char_code_point_t pCodePoint, FontGlyph * pOutGlyph, FontImageData * pOutGlyphImage );

		bool loadKerning( const CharCodePointPair & pCharCPPair, char_kerning_value_t * pOutKerning );

		void resetActiveGlyphCache();

		FreeTypeFontFace * getFace( uint32 pFontSize ) const;

		uint32 getLineHeight() const;

		static FreeTypeFontObjectPtr createFromMemory( const void * pFontData, size_t pFontDataSize );

	private:
		using FreeTypeFontFaceMap = std::unordered_map<font_size_t, FreeTypeFontFacePtr>;

		// Currently selected face.
		FreeTypeFontFace * _activeFace;

		// Instance of the FT library.
		FT_Library _ftLibraryInstance;

		// Loaded font data (from its font file). Used by FT_New_Memory_Face() when a new face is created.
		DynamicByteArray _ftFontData;

		// All faces created by this font, mapped by their size.
		FreeTypeFontFaceMap _ftFontFaceMap;
	};

} // namespace ts3

#endif // __TS3_ENGINE_RES_FTD_FREETYPE_FONT_OBJECT_H__
