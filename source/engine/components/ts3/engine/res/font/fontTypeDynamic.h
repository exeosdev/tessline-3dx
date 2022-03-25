
#pragma once

#ifndef __TS3_ENGINE_RES_FONT_TYPE_DYNAMIC_H__
#define __TS3_ENGINE_RES_FONT_TYPE_DYNAMIC_H__

#include "../font.h"
#include "../resourceLoader.h"
#include "ftdFreeTypeCommon.h"
#include <ts3/stdext/byteArray.h>

namespace ts3
{

	struct DynamicFontDesc : public FontCommonDesc
	{
		uint32 fixedLayersNum = 0;
		uint32 dynamicLayersNum = 0;
	};

	struct DynamicFontCreateInfo
	{
		DynamicFontDesc fontDesc;
		DynamicByteArray binaryFontData;
		math::Vec2u32 fontResolutionHint;
		std::vector<char_code_point_t> preloadGlyphSet;
	};

	class DynamicFont : public Font
	{
	public:
		DynamicFontDesc const mFontDesc;
	};

	class DynamicFontLoader : public ResourceLoader
	{
	public:
		FontHandle createFont( const DynamicFontCreateInfo & pCreateInfo );

	private:
		struct FTInternalFontData;

		FreeTypeFontObject * createFreeTypeFontObject( const DynamicFontCreateInfo & pFontCreateInfo );

		bool loadInternalFontData( FreeTypeFontObject * pFTFontObject, const DynamicFontCreateInfo & pFontCreateInfo, FTInternalFontData & pFontData );

		bool processFontInitData( FreeTypeFontObject * pFTFontObject, const DynamicFontCreateInfo & pFontCreateInfo, FTInternalFontData & pFontData );

		FontTextureCreateInfo setupTextureCreateInfo( const DynamicFontCreateInfo & pFontCreateInfo, FTInternalFontData & pFontData );

	private:
		struct FTFontLoaderPrivateData;
		std::unique_ptr<FTFontLoaderPrivateData> _privateData;
	};

	namespace gds
	{

		gds_size_t evalByteSize( const DynamicFontCreateInfo & pValue );
		gds_size_t serialize( byte * pOutputBuffer, const DynamicFontCreateInfo & pValue );
		gds_size_t deserialize( const byte * pInputData, DynamicFontCreateInfo & pValue );

	}

} // namespace ts3

#endif // __TS3_ENGINE_RES_FONT_TYPE_DYNAMIC_H__
