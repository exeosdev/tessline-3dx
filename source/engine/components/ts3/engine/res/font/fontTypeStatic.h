
#pragma once

#ifndef __TS3_ENGINE_RES_FONT_TYPE_STATIC_H__
#define __TS3_ENGINE_RES_FONT_TYPE_STATIC_H__

namespace ts3
{

	namespace rcdata
	{

		struct FontBMFBaseMetrics
		{
			uint16 lineHeight;
			uint16 base;
			uint32 pagesNum;
			math::Vec4u16 padding;
			math::Vec2u16 spacing;
			math::Vec2u16 scale;
			math::RGBAColorU8 pixelFormat;
			std::string faceName;
		};

		struct FontBMFPageDesc
		{
			uint32 id;
			math::Vec2u32 textureDimensions = { 0, 0 };
			DynamicMemoryBuffer imageData;
		};

		struct FontBMFCharDesc
		{
			char_code_point_t codePoint;
			math::Vec2u32 position;
			math::Vec2u16 size;
			math::Vec2u16 offset;
			uint16 xadvance;
			uint16 pageID;
		};

		struct FontBMFRCData
		{
			FontBaseInfo baseInfo;
			FontBMFBaseMetrics bmfMetrics;
			std::vector<FontBMFPageDesc> bmfPages;
			std::vector<FontBMFCharDesc> bmfCharArray;
			std::vector<CharKerningInfo> bmfKerningArray;
		};

	}

} // namespace ts3

#endif // __TS3_ENGINE_RES_FONT_TYPE_STATIC_H__
