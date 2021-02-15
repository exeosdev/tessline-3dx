
#include "font.h"
#include "fontTexture.h"
#include <ts3/gpuapi/commandContext.h>
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/resources/texture.h>

namespace ts3
{

	Font::Font( font_guid_t pFontGUID,
	            font_size_t pFontSize,
	            const FontTextureInfo & pFontTextureInfo,
	            std::unique_ptr<FontTexturePreloadCache> pTexturePreloadCache )
    : mFontGUID( pFontGUID )
	, mFontSize( pFontSize )
	, mTextureInfo( pFontTextureInfo )
	, _texturePreloadCache( std::move( pTexturePreloadCache ) )
	{}

	Font::~Font() = default;

	const FontGlyph * Font::getCharacterGlyph( char_code_point_t pCharCP )
	{
		return findCharacterGlyph( pCharCP );
	}

	char_kerning_value_t Font::getCharacterKerning( const CharCodePointPair & pCharCPPair )
	{
		return findCharacterKerning( pCharCPPair );
	}

	const FontGlyph * Font::findCharacterGlyph( char_code_point_t pCharCP ) const
	{
		auto glyphIter = _glyphMap.find( pCharCP );
		return ( glyphIter != _glyphMap.end() ) ? &( glyphIter->second ) : nullptr;
	}

	char_kerning_value_t Font::findCharacterKerning( const CharCodePointPair & pCharCPPair ) const
	{
		auto kerningIter = _kerningInfoArray.find( pCharCPPair, CharKerningInfoCmpEqual() );
		return ( kerningIter != _kerningInfoArray.end() ) ? kerningIter->kerningValue : 0;
	}

	FontTexturePreloadCache * Font::getTexturePreloadCache() const
	{
		return _texturePreloadCache.get();
	}

	bool Font::checkTexturePreloadCache() const
	{
		return _texturePreloadCache && !_texturePreloadCache->isEmpty();
	}

	void Font::addGlyph( const FontGlyph & pGlyph )
	{
		_glyphMap[pGlyph.codePoint] = pGlyph;
	}

	void Font::addKerningInfo( const CharKerningInfo & pKerningInfo )
	{
		_kerningInfoArray.insert( pKerningInfo );
	}

	void Font::setGlyphMap( std::unordered_map<char_code_point_t, FontGlyph> pGlyphMap )
	{
		_glyphMap = std::move( pGlyphMap );
	}

	void Font::setKerningInfoArray( std::vector<CharKerningInfo> pKerningInfoArray )
	{
		_kerningInfoArray.setData( std::move( pKerningInfoArray ) );
	}


	FontLoader::FontLoader( gpuapi::CommandContext & pGPUCommandContext )
	: _gpuCommandContext( &pGPUCommandContext )
	{}

	FontTextureInfo FontLoader::createFontTexture( const gpuapi::TextureCreateInfo & pGPUTextureCreateInfo )
	{
		if( !_gpuCommandContext || !pGPUTextureCreateInfo )
		{
			return nullptr;
		}

		auto gpuTexture = _gpuCommandContext->mGPUDevice.createTexture( pGPUTextureCreateInfo );
		if( !gpuTexture )
		{
			return nullptr;
		}

		FontTextureInfo fontTextureInfo;
		fontTextureInfo.texture = gpuTexture;
		fontTextureInfo.format = pGPUTextureCreateInfo.pixelFormat;
		fontTextureInfo.dimensionClass = pGPUTextureCreateInfo.dimensionClass;
		fontTextureInfo.textureArraySize = pGPUTextureCreateInfo.dimensions.arraySize;

		return fontTextureInfo;
	}

}
