
#pragma once

#ifndef __TS3_ENGINE_FONT_H__
#define __TS3_ENGINE_FONT_H__

#include "fontTexture.h"
#include <ts3/stdext/sortedArray.h>

namespace ts3
{

	using FontLoaderFactoryCallback = std::function<FontLoaderPtr()>;

	struct FontCreateInfo
	{
		font_type_t fontType;
		font_guid_t fontGUID;
		font_size_t fontSize;
		math::Vec2u32 textureDimensions = { 0, 0 };
		gpuapi::ETextureFormat textureFormat = gpuapi::ETextureFormat::R8_UNORM;
	};

	class Font
	{
		friend class FontLoader;

	protected:
		Font( font_guid_t pFontGUID,
		      font_size_t pFontSize,
		      const FontTextureInfo & pFontTextureInfo,
		      std::unique_ptr<FontTexturePreloadCache> pTexturePreloadCache );

	public:
		font_guid_t const mFontGUID;
		font_size_t const mFontSize;
		FontTextureInfo const mTextureInfo;

		virtual ~Font();

		virtual const FontGlyph * getCharacterGlyph( char_code_point_t pCharCP );
		virtual char_kerning_value_t getCharacterKerning( const CharCodePointPair & pCharCPPair );

		const FontGlyph * findCharacterGlyph( char_code_point_t pCharCP ) const;
		char_kerning_value_t findCharacterKerning( const CharCodePointPair & pCharCPPair ) const;

		FontTexturePreloadCache * getTexturePreloadCache() const;
		bool checkTexturePreloadCache() const;

	protected:
		void addGlyph( const FontGlyph & pGlyph );
		void addKerningInfo( const CharKerningInfo & pKerningInfo );
		void setGlyphMap( std::unordered_map<char_code_point_t, FontGlyph> pGlyphMap );
		void setKerningInfoArray( std::vector<CharKerningInfo> pKerningInfoArray );

	private:
		using GlyphMap = std::unordered_map<char_code_point_t, FontGlyph>;
		using KerningInfoArray = ts3::SortedArray<CharKerningInfo, CharKerningInfoCmpLess>;

		font_type_t _type;
		font_size_t _fontSize;
		GlyphMap _glyphMap;
		KerningInfoArray _kerningInfoArray;
		std::unique_ptr<FontTexturePreloadCache> _texturePreloadCache;
	};

	class FontLoader
	{
	public:
		explicit FontLoader( gpuapi::CommandContext & pGPUCommandContext );

		virtual ~FontLoader() = default;

		virtual FontHandle createFont( const FontCreateInfo & pFontCreateInfo ) = 0;

	protected:
		FontTextureInfo createFontTexture( const gpuapi::TextureCreateInfo & pGPUTextureCreateInfo );

	private:
		gpuapi::CommandContext * _gpuCommandContext;
	};

}

#endif // __TS3_ENGINE_FONT_H__
