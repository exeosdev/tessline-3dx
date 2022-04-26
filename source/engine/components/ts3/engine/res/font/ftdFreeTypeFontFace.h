
#pragma once

#ifndef __TS3_ENGINE_RES_FTD_FREETYPE_FONT_FACE_H__
#define __TS3_ENGINE_RES_FTD_FREETYPE_FONT_FACE_H__

#include "ftdFreeTypeCommon.h"
#include <unordered_map>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftglyph.h>

namespace ts3
{

	struct FreeTypeFontFaceDesc
	{
		uint32 faceIndex;
		uint32 fontSize;
		math::Vec2u32 fontResolutionHint;
	};

	struct FreeTypeFontFaceCreateInfo
	{
		FreeTypeFontFaceDesc faceDesc;
		const FT_Byte * inputData;
		FT_Long inputDataSize;
	};

	class FreeTypeFontFace
	{
	public:
		FreeTypeFontFace( std::nullptr_t );
		FreeTypeFontFace( FreeTypeFontObject * pFreeTypeFontObject, FT_Face pFTFace, const FreeTypeFontFaceDesc & pFaceDesc );
		~FreeTypeFontFace();

		explicit operator bool() const
		{
			return _fontObject != nullptr;
		}

		bool loadGlyphData( char_code_point_t pCodePoint, FT_Glyph * pOutGlyph, FT_Glyph_Metrics * pOutGlyphMetrics );

		bool loadGlyphImage( char_code_point_t pCodePoint, FT_BitmapGlyph * pOutGlyphBitmap );

		bool loadKerning( const CharCodePointPair & pCharCPPair, FT_Pos * pOutKerning );

		bool setFontResolution( const math::Vec2u32 & pFontResolution );

		void resetGlyphCache();

		FT_Glyph getGlyph( char_code_point_t pCodePoint ) const;

		FT_BitmapGlyph getGlyphBitmap( char_code_point_t pCodePoint ) const;

		const FT_Glyph_Metrics * getGlyphMetrics( char_code_point_t pCodePoint ) const;

		uint32 getFontSize() const
		{
			return _fontSize;
		}

		uint32 getLineHeight() const
		{
			return _ftFace->size->metrics.height / 64;
		}

		static FreeTypeFontFacePtr create( FreeTypeFontObject * pFreeTypeFontObject, const FreeTypeFontFaceCreateInfo & pFTFaceCreateInfo );

	private:
		struct CachedGlyphData
		{
			FT_Glyph ftGlyph;
			FT_Glyph_Metrics ftGlyphMetrics;
		};

		struct CachedGlyphBitmap
		{
			FT_BitmapGlyph ftBitmapGlyph;
		};

		using FTGlyphDataCache = std::unordered_map<char_code_point_t, CachedGlyphData>;
		using FTGlyphBitmapCache = std::unordered_map<char_code_point_t, CachedGlyphBitmap>;

		FreeTypeFontObject * _fontObject;
		FT_Face _ftFace;
		uint32 _faceIndex;
		uint32 _fontSize;
		math::Vec2u32 _fontResolution;
		FTGlyphDataCache _glyphDataCache;
		FTGlyphBitmapCache _glyphBitmapCache;
	};

} // namespace ts3

#endif // __TS3_ENGINE_RES_FTD_FREETYPE_FONT_FACE_H__
