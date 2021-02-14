
#ifndef __TS3EXT_RCSUPPORT_FREE_TYPE_LIBRARY_H__
#define __TS3EXT_RCSUPPORT_FREE_TYPE_LIBRARY_H__

#include "freeTypeCommon.h"

namespace ts3
{

	class FreeTypeFontObject
	{
	public:
		FT_Library const mFTLibrary;

		FreeTypeFontObject( std::nullptr_t );
		FreeTypeFontObject( FT_Library pFTLibrary, FreeTypeFontDataBuffer pFontData );
		~FreeTypeFontObject();

		explicit operator bool() const
		{
			return _ftLibraryInstance != nullptr;
		}

		bool loadFace( uint32 pFontSize );

		FreeTypeFontFace * queryFace( uint32 pFontSize );

		bool setActiveFace( uint32 pFontSize, const math::Vec2u32 & pFontResolution );

		bool loadGlyph( char_code_point_t pCodePoint, FontGlyph * pOutGlyph, FontImageData * pOutGlyphImage );

		bool loadKerning( const CharCodePointPair & pCharCPPair, char_kerning_value_t * pOutKerning );

		void resetActiveGlyphCache();

		uint32 getLineHeight() const;

		static FreeTypeFontObjectPtr create( const void * pFontData, size_t pFontDataSize );

	private:
		using FreeTypeFontFaceMap = std::unordered_map<font_size_t, FreeTypeFontFacePtr>;
		FreeTypeFontFace * _activeFace;
		FT_Library _ftLibraryInstance;
		FreeTypeFontDataBuffer _ftFontData;
		FreeTypeFontFaceMap _ftFontFaceMap;
	};

}

#endif // __TS3EXT_RCSUPPORT_FREE_TYPE_LIBRARY_H__
