
#include "ftdFreeTypeFontObject.h"
#include "ftdFreeTypeFontFace.h"

namespace ts3
{

	FreeTypeFontObject::FreeTypeFontObject( std::nullptr_t )
	: mFTLibrary( nullptr )
	, _ftLibraryInstance( nullptr )
	{}

	FreeTypeFontObject::FreeTypeFontObject( FT_Library pFTLibrary, DynamicByteArray pFontData )
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

	FreeTypeFontFace * FreeTypeFontObject::loadFace( uint32 pFontSize )
	{
		auto * facePtr = getFace( pFontSize );

		if( !facePtr )
		{
			FreeTypeFontFaceCreateInfo ftFaceCreateInfo{};
			ftFaceCreateInfo.faceDesc.faceIndex = 0;
			ftFaceCreateInfo.faceDesc.fontSize = pFontSize;
			ftFaceCreateInfo.faceDesc.fontResolutionHint = { 0, 0 };
			ftFaceCreateInfo.inputData = reinterpret_cast<const FT_Byte *>( _ftFontData.data() );
			ftFaceCreateInfo.inputDataSize = trunc_numeric_cast<FT_Long>( _ftFontData.size() );

			auto newFontFace = FreeTypeFontFace::create( this, ftFaceCreateInfo );
			if( newFontFace )
			{
				facePtr = newFontFace.get();
				_ftFontFaceMap.insert( { pFontSize, std::move( newFontFace ) } );
			}
		}

		return facePtr;
	}

	bool FreeTypeFontObject::setActiveFace( uint32 pFontSize, const math::Vec2u32 & pFontResolution )
	{
		if( auto * ftFace = getFace( pFontSize ) )
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
		if( _activeFace )
		{
			FT_Glyph ftGlyph = nullptr;
			FT_Glyph_Metrics ftGlyphMetrics;

			if( _activeFace->loadGlyphData( pCodePoint, &ftGlyph, &ftGlyphMetrics ) )
			{
				pOutGlyph->codePoint = pCodePoint;
				pOutGlyph->metrics.advance.x = ftGlyphMetrics.horiAdvance / 64;
				pOutGlyph->metrics.advance.y = ftGlyphMetrics.vertAdvance / 64;
				pOutGlyph->metrics.bearing.x = ftGlyphMetrics.horiBearingX / 64;
				pOutGlyph->metrics.bearing.y = ftGlyphMetrics.vertBearingX / 64;
				pOutGlyph->metrics.dimensions.x = ftGlyphMetrics.width / 64;
				pOutGlyph->metrics.dimensions.y = ftGlyphMetrics.height / 64;

				FT_BitmapGlyph ftBitmapGlyph = nullptr;

				if( _activeFace->loadGlyphImage( pCodePoint, &ftBitmapGlyph ) )
				{
					pOutGlyphImage->data = ftBitmapGlyph->bitmap.buffer;
					pOutGlyphImage->dataRowPitch = ftBitmapGlyph->bitmap.pitch;
					pOutGlyphImage->dataSize = ftBitmapGlyph->bitmap.pitch * ftBitmapGlyph->bitmap.rows;
					pOutGlyphImage->width = ftBitmapGlyph->bitmap.width;
					pOutGlyphImage->height = ftBitmapGlyph->bitmap.rows;
				}

				return true;
			}
		}

		return false;
	}

	bool FreeTypeFontObject::loadKerning( const CharCodePointPair & pCharCPPair, char_kerning_value_t * pOutKerning )
	{
		if( !_activeFace )
		{
			FT_Pos kerningValue = 0;

			if( _activeFace->loadKerning( pCharCPPair, &kerningValue ) )
			{
				*pOutKerning = static_cast<char_kerning_value_t>( kerningValue / 64 );
				return true;
			}
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

	FreeTypeFontFace * FreeTypeFontObject::getFace( uint32 pFontSize ) const
	{
		auto faceIter = _ftFontFaceMap.find( pFontSize );
		if( faceIter != _ftFontFaceMap.end() )
		{
			return faceIter->second.get();
		}
		return nullptr;
	}

	uint32 FreeTypeFontObject::getLineHeight() const
	{
		if( _activeFace )
		{
			return _activeFace->getLineHeight();
		}

		return 0;
	}

	FreeTypeFontObjectPtr FreeTypeFontObject::createFromMemory( const void * pFontData, size_t pFontDataSize )
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

		DynamicByteArray ftFontData;
		ftFontData.resize( pFontDataSize );
		ftFontData.setData( pFontData, pFontDataSize );

		return std::make_unique<FreeTypeFontObject>( ftLibrary, std::move( ftFontData ) );
	}

}
