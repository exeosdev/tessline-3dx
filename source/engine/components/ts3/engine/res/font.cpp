
#include "font.h"
#include <ts3/stdext/stlHelperAlgo.h>

namespace ts3
{

	bool operator==( const CharKerningInfo & pLhs, const CharKerningInfo & pRhs );
	bool operator==( const CharKerningInfo & pLhs, const CharCodePointPair & pRhs );

	bool operator<( const CharKerningInfo & pLhs, const CharKerningInfo & pRhs );
	bool operator<( const CharKerningInfo & pLhs, const CharCodePointPair & pRhs );


	Font::Font( const FontCommonDesc & pFontCommonDesc )
	: mFontCommonDesc( &pFontCommonDesc )
	{}

	Font::~Font() = default;

	const FontGlyph * Font::loadCharacterGlyph( char_code_point_t pCharCP )
	{
		return getCharacterGlyph( pCharCP );
	}

	char_kerning_value_t Font::loadCharacterKerning( const CharCodePointPair & pCharCPPair )
	{
		return getCharacterKerning( pCharCPPair );
	}

	const FontGlyph * Font::getCharacterGlyph( char_code_point_t pCharCP ) const
	{
		auto glyphIter = _glyphMap.find( pCharCP );
		return ( glyphIter != _glyphMap.end() ) ? &( glyphIter->second ) : nullptr;
	}

	char_kerning_value_t Font::getCharacterKerning( const CharCodePointPair & pCharCPPair ) const
	{
		auto kerningIter = _kerningInfoArray.find( pCharCPPair );
		return ( kerningIter != _kerningInfoArray.end() ) ? kerningIter->kerningValue : 0;
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

	void Font::setKerningData( std::vector<CharKerningInfo> pKerningData )
	{
		_kerningInfoArray.setData( std::move( pKerningData ) );
	}

	void Font::setKerningInfoArray( KerningInfoArray pKerningInfoArray )
	{
		_kerningInfoArray = std::move( pKerningInfoArray );
	}


	bool operator==( const CharKerningInfo & pLhs, const CharKerningInfo & pRhs )
	{
		return pLhs.codePointPair == pRhs.codePointPair;
	}

	bool operator==( const CharKerningInfo & pLhs, const CharCodePointPair & pRhs )
	{
		return pLhs.codePointPair == pRhs;
	}

	bool operator<( const CharKerningInfo & pLhs, const CharKerningInfo & pRhs )
	{
		return ( pLhs.codePointPair.first < pRhs.codePointPair.first ) ||
			   ( ( pLhs.codePointPair.first == pRhs.codePointPair.first ) && ( pLhs.codePointPair.second < pRhs.codePointPair.second ) );
	}

	bool operator<( const CharKerningInfo & pLhs, const CharCodePointPair & pRhs )
	{
		return ( pLhs.codePointPair.first < pRhs.first ) ||
			   ( ( pLhs.codePointPair.first == pRhs.first ) && ( pLhs.codePointPair.second < pRhs.second ) );
	}

}
