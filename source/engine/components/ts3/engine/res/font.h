
#pragma once

#ifndef __TS3_ENGINE_RES_FONT_H__
#define __TS3_ENGINE_RES_FONT_H__

#include "fontMetrics.h"
#include "fontTexture.h"
#include <ts3/stdext/sortedArray.h>
#include <unordered_map>

namespace ts3
{

    struct FontBaseInfo
    {
        EFontBaseType baseType = EFontBaseType::Unknown;
        std::string fontID;
        font_size_t fontSize = 0;
        math::Vec2u32 textureDimensions = { 0, 0 };
        gpuapi::ETextureFormat textureFormat = gpuapi::ETextureFormat::UNKNOWN;
        Bitmask<EFontPropertyFlags> propertyFlags = 0;

        TS3_FUNC_NO_DISCARD constexpr bool isValid() const
        {
            return ( baseType != EFontBaseType::Unknown ) &&
                   ( fontSize > 0 ) &&
                   ( textureDimensions.y > 0 ) &&
                   ( textureDimensions.y > 0 ) &&
                   ( textureFormat != gpuapi::ETextureFormat::UNKNOWN );
        }
    };

	class Font
	{
	public:
	    const FontBaseInfo * const mBaseInfo;

	protected:
	    Font( const FontBaseInfo & pBaseInfo );

	public:
		virtual ~Font();

		virtual const FontGlyph * loadCharacterGlyph( char_code_point_t pCharCP );

		virtual char_kerning_value_t loadCharacterKerning( const CharCodePointPair & pCharCPPair );

		const FontGlyph * getCharacterGlyph( char_code_point_t pCharCP ) const;

		char_kerning_value_t getCharacterKerning( const CharCodePointPair & pCharCPPair ) const;

	protected:
	    using GlyphMap = std::unordered_map<char_code_point_t, FontGlyph>;
	    using KerningInfoArray = SortedArray<CharKerningInfo>;

		void addGlyph( const FontGlyph & pGlyph );

		void addKerningInfo( const CharKerningInfo & pKerningInfo );

		void setGlyphMap( GlyphMap pGlyphMap );

		void setKerningData( std::vector<CharKerningInfo> pKerningData );
		void setKerningInfoArray( KerningInfoArray pKerningInfoArray );

	private:
		GlyphMap _glyphMap;
		KerningInfoArray _kerningInfoArray;
	};

}

#endif // __TS3_ENGINE_RES_FONT_H__