
#include "bitmapCommon.h"

namespace ts3
{

	static bool readBitmapMetadata( const void * pData, size_t pDataSize, BitmapMetadata * pOutMetadata );


	ImageData loadBitmapFromMemory( const void * pData, size_t pDataSize )
	{
		if( !pData || ( pDataSize == 0 ) )
		{
			return nullptr;
		}

		BitmapMetadata bitmapMetadata;

		if( !readBitmapMetadata( pData, pDataSize, &bitmapMetadata ) )
		{
			return nullptr;
		}

		ImageData imageData;
		imageData.formatInfo.dimensions.x = bitmapMetadata.dibInfoHeader.uVersion1.bcWidth;
		imageData.formatInfo.dimensions.y = bitmapMetadata.dibInfoHeader.uVersion1.bcHeight;
		imageData.formatInfo.bitDepth = 8;
		imageData.sizeInBytes = bitmapMetadata.dibInfoHeader.uVersion1.bcDataSize;

		if( bitmapMetadata.dibInfoHeader.uVersion1.bcColorDepth == 24 )
		{
			imageData.formatInfo.pixelLayout = EPixelDataLayout::BGR;
			imageData.formatInfo.pixelByteSize = 3;
		}
		else if( bitmapMetadata.dibInfoHeader.uVersion1.bcColorDepth == 32 )
		{
			imageData.formatInfo.pixelLayout = EPixelDataLayout::BGRA;
			imageData.formatInfo.pixelByteSize = 4;
		}

		const auto * imagePixelDataPtr = reinterpret_cast<const byte *>( pData ) + bitmapMetadata.coreFileHeader.dataOffset;
		const auto imagePixelRowPitch = imageData.formatInfo.dimensions.x * imageData.formatInfo.pixelByteSize;

		imageData.pixelBuffer.resize( imageData.sizeInBytes );

		for( uint32 rowIndex = 0; rowIndex < imageData.formatInfo.dimensions.y; ++rowIndex )
		{
			const auto * sourceData = imagePixelDataPtr + ( rowIndex * imagePixelRowPitch );
			const auto targetRowIndex = imageData.formatInfo.dimensions.y - rowIndex - 1;
			auto * targetBuffer = imageData.pixelBuffer.data() + ( targetRowIndex * imagePixelRowPitch );
			memCopy( targetBuffer, imagePixelRowPitch, sourceData, imagePixelRowPitch );
		}

		return imageData;
	}

	bool readBitmapMetadata( const void * pData, size_t pDataSize, BitmapMetadata * pOutMetadata )
	{
		ts3DebugAssert( pData );

		if( pDataSize < BITMAP_METADATA_SIZE_V1 )
		{
			return false;
		}

		const auto * imageData = reinterpret_cast<const byte *>( pData );

		BitmapMetadata metadata;
		metadata.dibInfoHeaderSize = 0;
		metadata.metadataSize = 0;
		metadata.formatVersion = BitmapImageFormatVersion::Unknown;

		memCopy( &( metadata.coreFileHeader ), sizeof( metadata.coreFileHeader ), imageData, BITMAP_FMTH_CORE_FILE_HEADER_SIZE );
		imageData += BITMAP_FMTH_CORE_FILE_HEADER_SIZE;

		if( ( metadata.coreFileHeader.controlSig[0] != 'B' ) || ( metadata.coreFileHeader.controlSig[1] != 'M' ) )
		{
			return false;
		}

		if( metadata.coreFileHeader.fileSize == 0 )
		{
			return false;
		}

		// Data offset is the size of core file header and the DIB info header. Using this simple math, we can
		// now deduce the size of the DIB header itself and deduce the format version to read the file properly.
		const auto dibInfoHeaderSize = metadata.coreFileHeader.dataOffset - BITMAP_FMTH_CORE_FILE_HEADER_SIZE;

		switch( dibInfoHeaderSize )
		{
			case BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V1:
				memCopy( &( metadata.dibInfoHeader.uVersion1 ), sizeof( metadata.dibInfoHeader.uVersion1 ), imageData, BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V1 );
				metadata.dibInfoHeaderSize = BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V1;
				metadata.metadataSize = BITMAP_METADATA_SIZE_V1;
				metadata.formatVersion = BitmapImageFormatVersion::BMPV1;
				break;

			case BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V2:
				memCopy( &( metadata.dibInfoHeader.uVersion2 ), sizeof( metadata.dibInfoHeader.uVersion2 ), imageData, BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V2 );
				metadata.dibInfoHeaderSize = BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V2;
				metadata.metadataSize = BITMAP_METADATA_SIZE_V2;
				metadata.formatVersion = BitmapImageFormatVersion::BMPV2;
				break;

			case BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V3:
				memCopy( &( metadata.dibInfoHeader.uVersion3 ), sizeof( metadata.dibInfoHeader.uVersion3 ), imageData, BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V3 );
				metadata.dibInfoHeaderSize = BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V3;
				metadata.metadataSize = BITMAP_METADATA_SIZE_V3;
				metadata.formatVersion = BitmapImageFormatVersion::BMPV3;
				break;

			case BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V4:
				memCopy( &( metadata.dibInfoHeader.uVersion4 ), sizeof( metadata.dibInfoHeader.uVersion4 ), imageData, BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V4 );
				metadata.dibInfoHeaderSize = BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V4;
				metadata.metadataSize = BITMAP_METADATA_SIZE_V4;
				metadata.formatVersion = BitmapImageFormatVersion::BMPV4;
				break;

			case BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V5:
				memCopy( &( metadata.dibInfoHeader.uVersion5 ), sizeof( metadata.dibInfoHeader.uVersion5 ), imageData, BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V5 );
				metadata.dibInfoHeaderSize = BITMAP_FMTH_DIB_INFO_HEADER_SIZE_V5;
				metadata.metadataSize = BITMAP_METADATA_SIZE_V5;
				metadata.formatVersion = BitmapImageFormatVersion::BMPV5;
				break;
		}

		if( metadata.formatVersion == BitmapImageFormatVersion::Unknown )
		{
			return false;
		}

		auto & v1DIBInfoHeader = metadata.dibInfoHeader.uVersion1;

		if( v1DIBInfoHeader.bcHeaderSize == 0 )
		{
			return false;
		}

		if( ( v1DIBInfoHeader.bcWidth == 0 ) || ( v1DIBInfoHeader.bcHeight == 0 ) || ( v1DIBInfoHeader.bcColorDepth == 0 ) )
		{
			return false;
		}

		if( ( v1DIBInfoHeader.bcColorDepth != 24 ) && ( v1DIBInfoHeader.bcColorDepth != 32 ) )
		{
			ts3DebugInterrupt();
			return false;
		}

		if( pOutMetadata )
		{
			if( v1DIBInfoHeader.bcDataSize == 0 )
			{
				v1DIBInfoHeader.bcDataSize = v1DIBInfoHeader.bcWidth * v1DIBInfoHeader.bcHeight * ( v1DIBInfoHeader.bcColorDepth / 8 );
			}

			memCopy( *pOutMetadata, metadata );
		}

		return true;
	}

}
