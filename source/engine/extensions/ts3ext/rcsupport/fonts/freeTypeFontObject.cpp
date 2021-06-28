
#include "freeTypeFontObject.h"
#include "freeTypeFontFace.h"

namespace ts3
{

	FreeTypeFontObject::FreeTypeFontObject( std::nullptr_t )
	: mFTLibrary( nullptr )
	, _ftLibraryInstance( nullptr )
	{}

	FreeTypeFontObject::FreeTypeFontObject( FT_Library pFTLibrary, FreeTypeFontDataBuffer pFontData )
	: mFTLibrary( pFTLibrary )
	, _activeFace( nullptr )
	, _ftLibraryInstance( pFTLibrary )
	, _ftFontData( std::move( pFontData ) )
	{}

	FreeTypeFontObject::~FreeTypeFontObject()
	{
	    if( !_ftFontFaceMap.empty() )
        {
	        // !! Do not forget about this !!
	        //
	        // All FT data (glyphs, faces, etc.) must be destroyed BEFORE FT_Done_FreeType() is called.
	        // FT_Done_FreeType() causes the whole library to invalidate and calls to most FT_* functions
	        // are no longer possible after that point. In cases of weird SIGSEGVs and NULL values inside
	        // FT objects - ensure everything has been released before FT_Done_FreeType() gets to work.

            _ftFontFaceMap.clear();
        }

		if( _ftLibraryInstance )
		{
			FT_Done_FreeType( _ftLibraryInstance );
			_ftLibraryInstance = nullptr;
		}
	}

	bool FreeTypeFontObject::loadFace( uint32 pFontSize )
	{
		auto faceIter = _ftFontFaceMap.find( pFontSize );
		if( faceIter != _ftFontFaceMap.end() )
		{
			return true;
		}

		FreeTypeFontFaceCreateInfo ftFaceCreateInfo{};
		ftFaceCreateInfo.faceDesc.faceIndex = 0;
		ftFaceCreateInfo.faceDesc.fontSize = pFontSize;
		ftFaceCreateInfo.faceDesc.fontResolutionHint = { 0, 0 };
		ftFaceCreateInfo.inputData = reinterpret_cast<const FT_Byte *>( _ftFontData.dataPtr() );
		ftFaceCreateInfo.inputDataSize = trunc_numeric_cast<FT_Long>( _ftFontData.size() );

		auto newFontFace = FreeTypeFontFace::create( this, ftFaceCreateInfo );
		if( newFontFace )
		{
			_ftFontFaceMap.insert( { pFontSize, std::move( newFontFace ) } );
			return true;
		}

		return false;
	}

	FreeTypeFontFace * FreeTypeFontObject::queryFace( uint32 pFontSize )
	{
		if( loadFace( pFontSize ) )
		{
			auto faceIter = _ftFontFaceMap.find( pFontSize );
			return faceIter->second.get();
		}

		return nullptr;
	}

	bool FreeTypeFontObject::setActiveFace( uint32 pFontSize, const math::Vec2u32 & pFontResolution )
	{
		if( auto * ftFace = queryFace( pFontSize ) )
		{
			if( ftFace->setFontResolution( pFontResolution ) )
			{
				_activeFace = ftFace;
				return true;
			}
		}

		return false;
	}

	bool FreeTypeFontObject::loadGlyph( char_code_point_t pCodePoint, FontGlyph * pOutGlyph, FontImageData * pOutGlyphImage )
	{
		if( !_activeFace )
		{
			return false;
		}

		FTGlyphData ftGlyphData;
		ftGlyphData.ftGlyph = nullptr;

		if( _activeFace->loadGlyphData( pCodePoint, &ftGlyphData ) )
		{
			pOutGlyph->codePoint = pCodePoint;
			pOutGlyph->metrics.advance.x = ftGlyphData.ftGlyphMetrics.horiAdvance / 64;
			pOutGlyph->metrics.advance.y = ftGlyphData.ftGlyphMetrics.vertAdvance / 64;
			pOutGlyph->metrics.bearing.x = ftGlyphData.ftGlyphMetrics.horiBearingX / 64;
			pOutGlyph->metrics.bearing.y = ftGlyphData.ftGlyphMetrics.vertBearingX / 64;
			pOutGlyph->metrics.dimensions.x = ftGlyphData.ftGlyphMetrics.width / 64;
			pOutGlyph->metrics.dimensions.y = ftGlyphData.ftGlyphMetrics.height / 64;

			FTGlyphImage ftGlyphImage;
			ftGlyphImage.ftGlyphBitmap = nullptr;

			if( _activeFace->loadGlyphImage( pCodePoint, &ftGlyphImage ) )
			{
				pOutGlyphImage->data = ftGlyphImage.ftGlyphBitmap->bitmap.buffer;
				pOutGlyphImage->dataRowPitch = ftGlyphImage.ftGlyphBitmap->bitmap.pitch;
				pOutGlyphImage->dataSize = ftGlyphImage.ftGlyphBitmap->bitmap.pitch * ftGlyphImage.ftGlyphBitmap->bitmap.rows;
				pOutGlyphImage->width = ftGlyphImage.ftGlyphBitmap->bitmap.width;
				pOutGlyphImage->height = ftGlyphImage.ftGlyphBitmap->bitmap.rows;
			}

			return true;
		}

		return false;
	}

	bool FreeTypeFontObject::loadKerning( const CharCodePointPair & pCharCPPair, char_kerning_value_t * pOutKerning )
	{
		if( !_activeFace )
		{
			return false;
		}

		FT_Pos kerningValue = 0;

		if( _activeFace->loadKerning( pCharCPPair, &kerningValue ) )
		{
			*pOutKerning = static_cast<char_kerning_value_t>( kerningValue / 64 );
			return true;
		}

		return false;
	}

	void FreeTypeFontObject::resetActiveGlyphCache()
	{
		if( _activeFace )
		{
			_activeFace->resetGlyphCache();
		}
	}

	uint32 FreeTypeFontObject::getLineHeight() const
	{
		if( _activeFace )
		{
			return _activeFace->getLineHeight();
		}

		return 0;
	}

	FreeTypeFontObjectPtr FreeTypeFontObject::create( const void * pFontData, size_t pFontDataSize )
	{
		if( !pFontData || ( pFontDataSize == 0 ) )
		{
			return nullptr;
		}

		FT_Library ftLibrary = nullptr;
		if( FT_Init_FreeType( &ftLibrary ) != FT_Err_Ok )
		{
			return nullptr;
		}

		FreeTypeFontDataBuffer ftFontData;
		ftFontData.resize( pFontDataSize );
		ftFontData.setData( pFontData, pFontDataSize );

		return std::make_unique<FreeTypeFontObject>( ftLibrary, std::move( ftFontData ) );
	}


	FreeTypeFontObjectPtr createFreeTypeFontObjectFromMemory( const void * pData, size_t pDataSize )
	{
		return FreeTypeFontObject::create( pData, pDataSize );
	}

}
