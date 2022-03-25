
#include "ftdFreeTypeFontFace.h"
#include "ftdFreeTypeFontObject.h"
#include <ts3/stdext/stlHelperAlgo.h>

namespace ts3
{

	FreeTypeFontFace::FreeTypeFontFace( std::nullptr_t )
	: _fontObject( nullptr )
	, _ftFace( nullptr )
	{}

	FreeTypeFontFace::FreeTypeFontFace( FreeTypeFontObject * pFTFontObject, FT_Face pFTFace, const FreeTypeFontFaceDesc & pFaceDesc )
	: _fontObject( pFTFontObject )
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

	bool FreeTypeFontFace::loadGlyphData( char_code_point_t pCodePoint, FT_Glyph * pOutGlyph, FT_Glyph_Metrics * pOutGlyphMetrics )
	{
		auto glyphDataIter = _glyphDataCache.find( pCodePoint );
		if( glyphDataIter == _glyphDataCache.end() )
		{
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

			CachedGlyphData glyphData;
			glyphData.ftGlyph = ftGlyph;
			glyphData.ftGlyphMetrics = _ftFace->glyph->metrics;

			auto insertIter = _glyphDataCache.insert( { pCodePoint, glyphData } );
			glyphDataIter = insertIter.first;
		}

		if( pOutGlyph )
		{
			*pOutGlyph = glyphDataIter->second.ftGlyph;
		}

		if( pOutGlyphMetrics )
		{
			*pOutGlyphMetrics = glyphDataIter->second.ftGlyphMetrics;
		}

		return true;

	}

	bool FreeTypeFontFace::loadGlyphImage( char_code_point_t pCodePoint, FT_BitmapGlyph * pOutGlyphBitmap )
	{
		auto glyphBitmapIter = _glyphBitmapCache.find( pCodePoint );
		if( glyphBitmapIter == _glyphBitmapCache.end() )
		{
			auto * ftGlyph = getGlyph( pCodePoint );
			if( !ftGlyph )
			{
				return false;
			}
			
			FT_BitmapGlyph bitmapGlyph = nullptr;

			if( ftGlyph->format == FT_GLYPH_FORMAT_BITMAP )
			{
				// Either glyph was loaded as a bitmap glyph or was converted to such.
				// We can safely downcast the glyph to its subclass (FT_BitmapGlyph).
				auto currentBitmapGlyph = reinterpret_cast<FT_BitmapGlyph>( ftGlyph );

				if( currentBitmapGlyph->bitmap.pixel_mode == FT_PIXEL_MODE_LCD )
				{
					bitmapGlyph = currentBitmapGlyph;
				}
			}

			if( !bitmapGlyph )
			{
				// Use FT_Glyph_To_Bitmap() to re-create the glyph as a bitmap glyph. This will destroy the current glyph.
				// During this process, bitmap data is automatically loaded into the glyph (so we can cast it).

				auto ftResult = FT_Glyph_To_Bitmap( &( ftGlyph ), FT_RENDER_MODE_LCD, nullptr, 1 );
				if( ftResult != FT_Err_Ok )
				{
					return false;
				}

				auto & cachedGlyphData = _glyphDataCache.at( pCodePoint );
				// Update the glyph inside the cache. The previous one has been invalidated already.
				cachedGlyphData.ftGlyph = ftGlyph;

				bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>( ftGlyph );
			}

			if( bitmapGlyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY )
			{
				FT_Bitmap ftBitmap;
				FT_Bitmap_Init( &ftBitmap );

				auto ftResult = FT_Bitmap_Convert( ftGlyph->library, &( bitmapGlyph->bitmap ), &ftBitmap, 8 );
				if( ftResult != FT_Err_Ok )
				{
					FT_Bitmap_Done( ftGlyph->library, &ftBitmap );
					return false;
				}

				ftResult = FT_Bitmap_Copy( ftGlyph->library, &ftBitmap, &( bitmapGlyph->bitmap ) );
				if( ftResult != FT_Err_Ok )
				{
					FT_Bitmap_Done( ftGlyph->library, &ftBitmap );
					return false;
				}

				FT_Bitmap_Done( ftGlyph->library, &ftBitmap );
			}

			CachedGlyphBitmap glyphBitmap;
			glyphBitmap.ftBitmapGlyph = bitmapGlyph;

			auto insertIter = _glyphBitmapCache.insert( { pCodePoint, glyphBitmap } );
			glyphBitmapIter = insertIter.first;
		}

		if( pOutGlyphBitmap )
		{
			*pOutGlyphBitmap = glyphBitmapIter->second.ftBitmapGlyph;
		}

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
		auto fontRes = pFontResolution;

		if( ( fontRes.x != 0 ) || ( fontRes.y != 0 ) )
		{
			if( fontRes.x == 0 )
			{
				fontRes.x = fontRes.y;
			}
			if( fontRes.y == 0 )
			{
				fontRes.y = fontRes.x;
			}
		}

		auto ftResult = FT_Set_Char_Size( _ftFace, 0, _fontSize * 64, fontRes.x, fontRes.y );
		if( ftResult != FT_Err_Ok )
		{
			return false;
		}

		_fontResolution = fontRes;

		return true;
	}

	void FreeTypeFontFace::resetGlyphCache()
	{
		for( auto & ftGlyph : _glyphDataCache )
		{
			FT_Done_Glyph( ftGlyph.second.ftGlyph );
		}

		_glyphDataCache.clear();
		_glyphBitmapCache.clear();
	}

	FT_Glyph FreeTypeFontFace::getGlyph( char_code_point_t pCodePoint ) const
	{
		if( auto * cachedGlyphData = stdx::getMapValuePtrOrNull( _glyphDataCache, pCodePoint ) )
		{
			return cachedGlyphData->ftGlyph;
		}
		return nullptr;
	}

	FT_BitmapGlyph FreeTypeFontFace::getGlyphBitmap( char_code_point_t pCodePoint ) const
	{
		if( auto * cachedGlyphBitmap = stdx::getMapValuePtrOrNull( _glyphBitmapCache, pCodePoint ) )
		{
			return cachedGlyphBitmap->ftBitmapGlyph;
		}
		return nullptr;
	}

	const FT_Glyph_Metrics * FreeTypeFontFace::getGlyphMetrics( char_code_point_t pCodePoint ) const
	{
		if( auto * cachedGlyphData = stdx::getMapValuePtrOrNull( _glyphDataCache, pCodePoint ) )
		{
			return &( cachedGlyphData->ftGlyphMetrics );
		}
		return nullptr;
	}

	FreeTypeFontFacePtr FreeTypeFontFace::create( FreeTypeFontObject * pFTFontObject, const FreeTypeFontFaceCreateInfo & pFTFaceCreateInfo )
	{
		FT_Face ftFontFace = nullptr;

		auto ftResult = FT_New_Memory_Face( pFTFontObject->mFTLibrary,
											pFTFaceCreateInfo.inputData,
											pFTFaceCreateInfo.inputDataSize,
											-1,
											&ftFontFace );

		// Check for potential errors and validate the face itself. If face has 'num_faces'
		// set to 0 (for any reason), it is useless for the purpose of glyph rendering.
		if( ( ftResult != FT_Err_Ok ) || !ftFontFace || ( ftFontFace->num_faces == 0 ) )
		{
			return nullptr;
		}

		ftResult = FT_New_Memory_Face( pFTFontObject->mFTLibrary,
									   pFTFaceCreateInfo.inputData,
									   pFTFaceCreateInfo.inputDataSize,
									   pFTFaceCreateInfo.faceDesc.faceIndex,
									   &ftFontFace );

		// If that fails, it is most likely due to an invalid input data.
		if( ftResult != FT_Err_Ok )
		{
			return nullptr;
		}

		auto fontFacePtr =  std::make_unique<FreeTypeFontFace>( pFTFontObject, ftFontFace, pFTFaceCreateInfo.faceDesc );

		// Set the resolution using the value provided in the face desc.
		// Not, that this handles {0,0} automatically (in such case it should never fail).
		if( !fontFacePtr->setFontResolution( pFTFaceCreateInfo.faceDesc.fontResolutionHint ) )
		{
			return nullptr;
		}

		return fontFacePtr;
	}

}
