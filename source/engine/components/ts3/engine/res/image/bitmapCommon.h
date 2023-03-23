
#ifndef __TS3_ENGINE_BITMAP_COMMON_H__
#define __TS3_ENGINE_BITMAP_COMMON_H__

#include <ts3/engine/res/imageCommon.h>

namespace ts3
{

	enum class BitmapImageFormatVersion : enum_default_value_t
	{
		BMPV1,
		BMPV2,
		BMPV3,
		BMPV4,
		BMPV5,
		Unknown
	};

#pragma pack( push, 1 )

	struct BitmapCoreFileHeader
	{
		byte   controlSig[2];
		uint32 fileSize;
		uint16 reserved[2];
		uint32 dataOffset;
	};

	struct BitmapDIBInfoHeaderV1
	{
		uint32 bcHeaderSize;
		int32  bcWidth;
		int32  bcHeight;
		uint16 bcColorPlanesNum;
		uint16 bcColorDepth;
		uint32 bcCompression;
		uint32 bcDataSize;
		uint32 bcPpmX;
		uint32 bcPpmY;
		uint32 bcPaletteUsedColorsNum;
		uint32 bcPaletteImportantColorsNum;
	};

	struct BitmapDIBInfoHeaderV2 : public BitmapDIBInfoHeaderV1
	{
		uint32 v2MaskRed;
		uint32 v2MaskGreen;
		uint32 v2MaskBlue;
	};

	struct BitmapDIBInfoHeaderV3 : public BitmapDIBInfoHeaderV2
	{
		uint32 v3MaskAlpha;
	};

	struct BitmapDIBInfoHeaderV4 : public BitmapDIBInfoHeaderV3
	{
		struct RGBEndpoints
		{
			math::Vec3i32 red;
			math::Vec3i32 green;
			math::Vec3i32 blue;
		};

		struct RGBGamma
		{
			uint32 red;
			uint32 green;
			uint32 blue;
		};

		uint32	     v4CSType;
		RGBEndpoints v4RGBEndpoints;
		RGBGamma     v4RGBGamma;
	};

	struct BitmapDIBInfoHeaderV5 : public BitmapDIBInfoHeaderV4
	{
		uint32 v5Intent;
		uint32 v5ProfileData;
		uint32 v5ProfileSize;
		uint32 v5Reserved;
	};

	struct BitmapMetadata
	{
		union DIBInfoHeader
		{
			BitmapDIBInfoHeaderV1 uVersion1;
			BitmapDIBInfoHeaderV2 uVersion2;
			BitmapDIBInfoHeaderV3 uVersion3;
			BitmapDIBInfoHeaderV4 uVersion4;
			BitmapDIBInfoHeaderV5 uVersion5;
		};

		BitmapCoreFileHeader coreFileHeader;
		DIBInfoHeader dibInfoHeader;
		uint32 dibInfoHeaderSize;
		uint32 metadataSize;
		BitmapImageFormatVersion formatVersion;
	};

#pragma pack( pop )

	enum : size_t
	{
		BITMAP_FMTH_CORE_FILE_HEADER_SIZE   = sizeof( BitmapCoreFileHeader ),
		BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V1 = sizeof( BitmapDIBInfoHeaderV1 ),
		BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V2 = sizeof( BitmapDIBInfoHeaderV2 ),
		BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V3 = sizeof( BitmapDIBInfoHeaderV3 ),
		BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V4 = sizeof( BitmapDIBInfoHeaderV4 ),
		BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V5 = sizeof( BitmapDIBInfoHeaderV5 ),
		BITMAP_METADATA_SIZE_V1             = BITMAP_FMTH_CORE_FILE_HEADER_SIZE + BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V1,
		BITMAP_METADATA_SIZE_V2             = BITMAP_FMTH_CORE_FILE_HEADER_SIZE + BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V2,
		BITMAP_METADATA_SIZE_V3             = BITMAP_FMTH_CORE_FILE_HEADER_SIZE + BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V3,
		BITMAP_METADATA_SIZE_V4             = BITMAP_FMTH_CORE_FILE_HEADER_SIZE + BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V4,
		BITMAP_METADATA_SIZE_V5             = BITMAP_FMTH_CORE_FILE_HEADER_SIZE + BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V5
	};

	ImageData loadBitmapFromMemory( const void * pData, size_t pDataSize );

} // namespace ts3

#endif // __TS3_ENGINE_BITMAP_COMMON_H__
