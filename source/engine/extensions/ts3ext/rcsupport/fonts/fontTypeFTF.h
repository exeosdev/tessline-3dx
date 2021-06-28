
#pragma once

#ifndef __TS3EXT_RCSUPPORT_FONT_TYPE_FTF_H__
#define __TS3EXT_RCSUPPORT_FONT_TYPE_FTF_H__

#include <ts3/engine/rcdata/font.h>
#include <ts3/core/graphics/rectAllocator.h>
#include "freeTypeFontObject.h"

namespace ts3
{

	class FreeTypeFont;
	class FreeTypeFontLoader;

	enum : font_type_t
	{
		FONT_TYPE_FTF = 0xFF00
	};

	struct FreeTypeFontCreateInfo : public FontCreateInfo
	{
		std::string freeTypeFontID;
		std::string freeTypeFontFile;
		uint32 dynamicLayersNum = 0;
		math::Vec2u32 fontResolutionHint = { 0, 0 };
		CharCodePointSet preloadGlyphSet;
	};

	struct FreeTypeGlyphData
	{
		FontGlyph glyph;
		FontImageData glyphImage;
	};

	struct FreeTypeFontData
	{
		struct TextureLayerData
		{
			uint32 layerIndex;
			DynamicMemoryBuffer initDataBuffer;
		};

		std::vector<FreeTypeGlyphData> glyphArray;
		std::vector<CharKerningInfo> charKerningArray;
		std::vector<TextureLayerData> textureLayerDataArray;
		std::unordered_map<char_code_point_t, FontGlyph> glyphMap;

		explicit operator bool() const
		{
			return !glyphArray.empty();
		}
	};

	class FreeTypeFont : public Font
	{
		friend class FreeTypeFontLoader;

	public:
		FreeTypeFont( font_guid_t pFontGUID,
		              font_size_t pFontSize,
		              const FontTextureInfo & pFontTextureInfo );

		virtual ~FreeTypeFont();
	};

	class FreeTypeFontLoader : public FontLoader
	{
	public:
		explicit FreeTypeFontLoader( gpuapi::CommandContext & pGPUCommandContext );

		virtual ~FreeTypeFontLoader();

		virtual FontHandle createFont( const FontCreateInfo & pFontCreateInfo ) override;

	private:
		FreeTypeFontData loadFontData( FreeTypeFontObject * pFTFontObject,
		                               const FreeTypeFontCreateInfo & pFontCreateInfo );

		void initializeTextureData( FreeTypeFontObject * pFTFontObject,
		                            FreeTypeFontData & pFTFontData,
		                            const FreeTypeFontCreateInfo & pFontCreateInfo );

		FreeTypeFontObject * createFreeTypeFontObject( const FreeTypeFontCreateInfo & pFontCreateInfo );

        static gpuapi::TextureCreateInfo getTextureCreateInfo( const FreeTypeFontData & pFTFontData,
                                                               const FreeTypeFontCreateInfo & pFontCreateInfo );

	private:
		struct FTFontInstance
		{
			std::string ftFontID;
			std::string ftFontFile;
			FreeTypeFontObjectPtr  ftFontObject;
		};
		using FTFontInstanceMap = std::unordered_map<std::string, FTFontInstance>;
		FTFontInstanceMap _ftFontInstanceMap;
	};

}

#endif // __TS3EXT_RCSUPPORT_FONT_TYPE_FTF_H__
