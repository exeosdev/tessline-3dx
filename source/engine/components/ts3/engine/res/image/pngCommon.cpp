
#include "pngCommon.h"

namespace ts3
{

	namespace pnglib
	{

		bool init( PngReadState * pPngState )
		{

			png_struct * pngStructPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );

			if( !pngStructPtr )
			{
				return false;
			}

			png_info * pngInfoPtr = png_create_info_struct( pngStructPtr );

			if( !pngInfoPtr )
			{
				png_destroy_read_struct( &pngStructPtr, nullptr, nullptr );
				return false;
			}

			pPngState->readStruct = pngStructPtr;
			pPngState->infoStruct = pngInfoPtr;

			return true;
		}

		bool init( PngWriteState * pPngState )
		{
			png_struct * pngStructPtr = png_create_write_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );

			if( !pngStructPtr )
			{
				return false;
			}

			png_info * pngInfoPtr = png_create_info_struct( pngStructPtr );

			if( !pngInfoPtr )
			{
				png_destroy_write_struct( &pngStructPtr, nullptr );
				return false;
			}

			pPngState->writeStruct = pngStructPtr;
			pPngState->infoStruct = pngInfoPtr;

			return true;
		}

		void release( PngReadState * pPngState )
		{
			png_destroy_info_struct( pPngState->readStruct, &pPngState->infoStruct );
			pPngState->infoStruct = nullptr;

			png_destroy_read_struct( &pPngState->readStruct, nullptr, nullptr );
			pPngState->readStruct = nullptr;
		}

		void release( PngWriteState * pPngState )
		{
			png_destroy_info_struct( pPngState->writeStruct, &pPngState->infoStruct );
			pPngState->infoStruct = nullptr;

			png_destroy_write_struct( &pPngState->writeStruct, nullptr );
			pPngState->writeStruct = nullptr;
		}

		bool imageDataValidate( const byte * pRawData, native_uint pSize )
		{
			const auto sigDataSize = getMinOf( pSize, 8u );
			int sigCheckResult = png_sig_cmp( pRawData, 0, sigDataSize );

			if( sigCheckResult == 0 )
			{
				// if( sigDataSize <= 5 )
				// {
				// 	// Possibly valid
				// }
				// else
				// {
				// 	// Most likely valid
				// }
				return true;
			}

			return false;
		}

		bool imageDataRead( PngReadState * pPngState, ImageData * pOutput )
		{
			auto * pngReadStruct = pPngState->readStruct;
			auto * pngInfoStruct = pPngState->infoStruct;

			png_read_info( pngReadStruct, pngInfoStruct );

			png_uint_32 imgWidth = png_get_image_width( pngReadStruct, pngInfoStruct );
			png_uint_32 imgHeight = png_get_image_height( pngReadStruct, pngInfoStruct );
			png_byte imgBitDepth = png_get_bit_depth( pngReadStruct, pngInfoStruct );
			png_byte imgColorType = png_get_color_type( pngReadStruct, pngInfoStruct );
			png_byte imgChannels = png_get_channels( pngReadStruct, pngInfoStruct );

			switch( imgColorType )
			{
			case PNG_COLOR_TYPE_PALETTE:
			{
				png_set_palette_to_rgb( pngReadStruct );
				imgColorType = PNG_COLOR_TYPE_RGB;
				imgChannels = 3;
			}
			break;

			case PNG_COLOR_TYPE_GRAY:
			{
				if( imgBitDepth < 8 )
				{
					png_set_expand_gray_1_2_4_to_8( pngReadStruct );
				}

				imgBitDepth = 8;
			}
			break;
			}

			if( png_get_valid( pngReadStruct, pngInfoStruct, PNG_INFO_tRNS ) )
			{
				png_set_tRNS_to_alpha( pngReadStruct );
				imgChannels += 1;
			}

			if( imgBitDepth == 16 )
			{
				png_set_strip_16( pngReadStruct );
			}

			auto imgRowsStride = ( ( imgWidth * imgChannels * imgBitDepth ) / 8 );
			auto imageDataSize = imgRowsStride * imgHeight;

			pOutput->pixelBuffer.resize( imageDataSize );

			auto * imgDataPtr = pOutput->pixelBuffer.data();
			auto ** rowsArray = reinterpret_cast<byte **>( malloc( sizeof( byte * ) * imgHeight ) );

			for( native_uint rowIndex = 0; rowIndex < imgHeight; ++rowIndex )
			{
				const auto dataStride = ( imgHeight - rowIndex - 1 ) * imgRowsStride;
				rowsArray[rowIndex] = imgDataPtr + dataStride;
			}

			png_read_image( pngReadStruct, rowsArray );

			free( rowsArray );

			pOutput->formatInfo.dimensions.x = imgWidth;
			pOutput->formatInfo.dimensions.y = imgHeight;

			pOutput->formatInfo.bitDepth = imgBitDepth;
			pOutput->formatInfo.pixelByteSize = imgChannels * imgBitDepth / 8;
			pOutput->sizeInBytes = imageDataSize;

			switch( imgColorType )
			{
			case PNG_COLOR_TYPE_GRAY:
				pOutput->formatInfo.pixelLayout = EPixelDataLayout::Red;
				break;

			case PNG_COLOR_TYPE_GRAY_ALPHA:
				pOutput->formatInfo.pixelLayout = EPixelDataLayout::RG;
				break;

			case PNG_COLOR_TYPE_RGB:
				pOutput->formatInfo.pixelLayout = EPixelDataLayout::RGB;
				break;

			case PNG_COLOR_TYPE_RGB_ALPHA:
				pOutput->formatInfo.pixelLayout = EPixelDataLayout::RGBA;
				break;
			}

			return true;
		}

		bool imageDataWrite( PngWriteState * pPngState, const ImageData * pInput )
		{
			return false;
		}

		struct PngStreamState
		{
			const byte * pngDataPtr = nullptr;

			native_uint pngDataSize = 0;

			native_uint readOffset = 0;
		};

		void readFromStream( png_struct * pPngStruct, png_byte * pBuffer, png_size_t pSize )
		{
			void * readObjPtr = png_get_io_ptr( pPngStruct );
			auto * pngStream = reinterpret_cast<PngStreamState *>( readObjPtr );

			ts3DebugAssert( pSize + pngStream->readOffset <= pngStream->pngDataSize );

			memCopyUnchecked( pBuffer,  pngStream->pngDataSize -  pngStream->readOffset, pngStream->pngDataPtr, pSize );

			pngStream->readOffset += pSize;
			pngStream->pngDataPtr += pSize;
		}

	};

	ImageData loadPNGFromMemory( const void * pData, size_t pDataSize )
	{
		pnglib::PngReadState pngReadState;

		if( !pnglib::init(&pngReadState) )
		{
			ts3DebugInterrupt();
			return {};
		}

		pnglib::PngStreamState pngStream;
		pngStream.pngDataPtr = reinterpret_cast<const byte *>( pData );
		pngStream.pngDataSize = pDataSize;

		byte signatureBuffer[8];
		memCopyUnchecked( signatureBuffer, 8, pData, 8 );

		if( png_sig_cmp( signatureBuffer, 0, 8 ) !=  0)
		{
			ts3DebugInterrupt();
			return {};
		}

		png_set_read_fn( pngReadState.readStruct, &pngStream, pnglib::readFromStream ) ;
		png_set_sig_bytes( pngReadState.readStruct, 0 );
		png_set_alpha_mode_fixed( pngReadState.readStruct, PNG_ALPHA_PREMULTIPLIED, PNG_GAMMA_sRGB );

		ImageData imageData;
		pnglib::imageDataRead( &pngReadState, &imageData );

		pnglib::release( &pngReadState );

		return imageData;
	}

} // namespace ts3
