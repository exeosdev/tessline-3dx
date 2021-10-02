
#ifndef __TS3EXT_RCSUPPORT_FREE_TYPE_FONT_FACE_H__
#define __TS3EXT_RCSUPPORT_FREE_TYPE_FONT_FACE_H__

#include "freeTypeCommon.h"
#include <unordered_map>

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

		bool loadGlyphData( char_code_point_t pCodePoint, FTGlyphData * pOutFTGlyphData );

		bool loadGlyphImage( char_code_point_t pCodePoint, FTGlyphImage * pOutFTGlyphImage );

		bool loadKerning( const CharCodePointPair & pCharCPPair, FT_Pos * pOutKerning );

		bool setFontResolution( const math::Vec2u32 & pFontResolution );

		void resetGlyphCache();

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
		using FTGlyphCache = std::unordered_map<char_code_point_t, FTGlyphData>;

		FreeTypeFontObject * _fontObject = nullptr;
		FT_Face _ftFace;
		uint32 _faceIndex;
		uint32 _fontSize;
		math::Vec2u32 _fontResolution;
		FTGlyphCache _glyphCache;
	};

}

#endif // __TS3EXT_RCSUPPORT_FREE_TYPE_FONT_FACE_H__
