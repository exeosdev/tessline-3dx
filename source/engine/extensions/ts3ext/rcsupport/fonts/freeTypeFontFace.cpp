
#include "freeTypeFontFace.h"
#include "freeTypeFontObject.h"

namespace ts3
{

	FreeTypeFontFace::FreeTypeFontFace( std::nullptr_t )
	: _fontObject( nullptr )
	, _ftFace( nullptr )
	{}

	FreeTypeFontFace::FreeTypeFontFace( FreeTypeFontObject * pFreeTypeFontObject, FT_Face pFTFace, const FreeTypeFontFaceDesc & pFaceDesc )
	: _fontObject( pFreeTypeFontObject )
	, _ftFace( pFTFace )
	, _faceIndex( pFaceDesc.faceIndex )
	, _fontSize( pFaceDesc.fontSize )
	{}

	FreeTypeFontFace::~FreeTypeFontFace()
	{
		if( _fontObject )
		{
			resetGlyphCache();

			if( _ftFace )
			{
				FT_Done_Face( _ftFace );
				_ftFace = nullptr;
			}
		}
	}

	bool FreeTypeFontFace::loadGlyphData( char_code_point_t pCodePoint, FTGlyphData * pOutFTGlyphData )
	{
		auto glyphCacheIter = _glyphCache.find( pCodePoint );
		if( glyphCacheIter != _glyphCache.end() )
		{
			*pOutFTGlyphData = glyphCacheIter->second;
			return true;
		}

		FT_UInt ftGlyphIndex = FT_Get_Char_Index( _ftFace, pCodePoint );
		if( ftGlyphIndex == 0 )
		{
			return false;
		}

		auto ftResult = FT_Load_Glyph( _ftFace, ftGlyphIndex, FT_LOAD_COLOR );
		if( ftResult != FT_Err_Ok )
		{
			return false;
		}

		FT_Glyph ftGlyph = nullptr;

		ftResult = FT_Get_Glyph( _ftFace->glyph, &ftGlyph );
		if( ftResult != FT_Err_Ok )
		{
			return false;
		}

		pOutFTGlyphData->ftGlyph = ftGlyph;
		pOutFTGlyphData->ftGlyphMetrics = _ftFace->glyph->metrics;

		_glyphCache[pCodePoint] = *pOutFTGlyphData;

		return true;

	}

	bool FreeTypeFontFace::loadGlyphImage( char_code_point_t pCodePoint, FTGlyphImage * pOutFTGlyphImage )
	{
		FTGlyphData ftGlyphData;
		if( !loadGlyphData( pCodePoint, &ftGlyphData ) )
		{
			return false;
		}

		FT_BitmapGlyph bitmapGlyph = nullptr;

		if( ftGlyphData.ftGlyph->format == FT_GLYPH_FORMAT_BITMAP )
		{
			// Either glyph was loaded as a bitmap glyph or was converted to such.
			// We can safely downcast the glyph to its subclass (FT_BitmapGlyph).
			auto currentBitmapGlyph = reinterpret_cast<FT_BitmapGlyph>( ftGlyphData.ftGlyph );

			if( currentBitmapGlyph->bitmap.pixel_mode == FT_PIXEL_MODE_LCD )
			{
				bitmapGlyph = currentBitmapGlyph;
			}
		}

		if( !bitmapGlyph )
		{
			// FT_Glyph_To_Bitmap will destroy the glyph object (which is an FT_Glyph now)
			// and re-create it as FT_BitmapGlyph. Then the bitmap data is loaded into it.

			auto ftResult = FT_Glyph_To_Bitmap( &( ftGlyphData.ftGlyph ), FT_RENDER_MODE_LCD, nullptr, 1 );
			if( ftResult != FT_Err_Ok )
			{
				return false;
			}

			bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>( ftGlyphData.ftGlyph );
			_glyphCache[pCodePoint] = ftGlyphData;
		}

		if( bitmapGlyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY )
		{
			FT_Bitmap ftBitmap;
			FT_Bitmap_Init( &ftBitmap );

			auto ftResult = FT_Bitmap_Convert( ftGlyphData.ftGlyph->library, &( bitmapGlyph->bitmap ), &ftBitmap, 8 );
			if( ftResult != FT_Err_Ok )
			{
				FT_Bitmap_Done( ftGlyphData.ftGlyph->library, &ftBitmap );
				return false;
			}

			ftResult = FT_Bitmap_Copy( ftGlyphData.ftGlyph->library, &ftBitmap, &( bitmapGlyph->bitmap ) );
			if( ftResult != FT_Err_Ok )
			{
				FT_Bitmap_Done( ftGlyphData.ftGlyph->library, &ftBitmap );
				return false;
			}

			FT_Bitmap_Done( ftGlyphData.ftGlyph->library, &ftBitmap );
		}

		pOutFTGlyphImage->ftGlyphBitmap = bitmapGlyph;

		return true;
	}

	bool FreeTypeFontFace::loadKerning( const CharCodePointPair & pCharCPPair, FT_Pos * pOutKerning )
	{
		FT_UInt ftFirstGlyphIndex = FT_Get_Char_Index( _ftFace, pCharCPPair.first );
		FT_UInt ftSecondGlyphIndex = FT_Get_Char_Index( _ftFace, pCharCPPair.second );

		FT_Vector kerningVector;
		kerningVector.x = 0;
		kerningVector.y = 0;

		auto ftResult = FT_Get_Kerning( _ftFace, ftFirstGlyphIndex, ftSecondGlyphIndex, FT_KERNING_DEFAULT, &kerningVector );
		if( ( ftResult != FT_Err_Ok ) || ( kerningVector.x == 0 ) )
		{
			return false;
		}

		*pOutKerning = kerningVector.x;

		return true;
	}

	bool FreeTypeFontFace::setFontResolution( const math::Vec2u32 & pFontResolution )
	{
		auto fontResolution = pFontResolution;

		if( ( fontResolution.x != 0 ) && ( fontResolution.y != 0 ) )
		{
			if( fontResolution.x == 0 )
			{
				fontResolution.x = fontResolution.y;
			}
			if( fontResolution.y == 0 )
			{
				fontResolution.y = fontResolution.x;
			}
		}

		auto ftResult = FT_Set_Char_Size( _ftFace,
		                                  0,
		                                  _fontSize * 64,
		                                  fontResolution.x,
		                                  fontResolution.y );

		if( ftResult != FT_Err_Ok )
		{
			return false;
		}

		_fontResolution = fontResolution;

		return true;
	}

	void FreeTypeFontFace::resetGlyphCache()
	{
		for( auto & ftGlyph : _glyphCache )
		{
			FT_Done_Glyph( ftGlyph.second.ftGlyph );
		}
		_glyphCache.clear();
	}

	FreeTypeFontFacePtr FreeTypeFontFace::create( FreeTypeFontObject * pFreeTypeFontObject, const FreeTypeFontFaceCreateInfo & pFTFaceCreateInfo )
	{
		FT_Face ftFontFace = nullptr;

		auto ftResult = FT_New_Memory_Face( pFreeTypeFontObject->mFTLibrary,
		                                    pFTFaceCreateInfo.inputData,
		                                    pFTFaceCreateInfo.inputDataSize,
		                                    -1,
		                                    &ftFontFace );

		if( ( ftResult != FT_Err_Ok ) || !ftFontFace || ( ftFontFace->num_faces == 0 ) )
		{
			return nullptr;
		}

		ftResult = FT_New_Memory_Face( pFreeTypeFontObject->mFTLibrary,
		                               pFTFaceCreateInfo.inputData,
		                               pFTFaceCreateInfo.inputDataSize,
		                               pFTFaceCreateInfo.faceDesc.faceIndex,
		                               &ftFontFace );

		if( ftResult != FT_Err_Ok )
		{
			return nullptr;
		}

		auto fontFacePtr =  std::make_unique<FreeTypeFontFace>( pFreeTypeFontObject, ftFontFace, pFTFaceCreateInfo.faceDesc );

		if( !fontFacePtr->setFontResolution( pFTFaceCreateInfo.faceDesc.fontResolutionHint ) )
		{
			return nullptr;
		}

		return fontFacePtr;
	}

}
