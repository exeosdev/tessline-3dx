
#ifndef __TS3_ENGINE_PNG_COMMON_H__
#define __TS3_ENGINE_PNG_COMMON_H__

#include <ts3/engine/res/imageCommon.h>

#include <png/png.h>
#include <png/pngdebug.h>

namespace ts3
{

	namespace pnglib
	{

		struct PngReadState
		{
			png_struct * readStruct = nullptr;
			png_info * infoStruct = nullptr;
		};


		struct PngWriteState
		{
			png_struct * writeStruct = nullptr;
			png_info * infoStruct = nullptr;
		};

		bool init( PngReadState * pPngState );

		bool init( PngWriteState * pPngState );

		void release( PngReadState * pPngState );

		void release( PngWriteState * pPngState );

		bool imageDataValidate( const byte * pRawData, native_uint pSize );

		bool imageDataRead( PngReadState * pPngState, ImageData * pOutput );

		bool imageDataWrite( PngWriteState * pPngState, const ImageData * pInput );

		void readFromStream( png_struct * pPngStruct, png_byte * pBuffer, png_size_t pSize );

	};

	ImageData loadPNGFromMemory( const void * pData, size_t pDataSize );

} // namespace ts3

#endif // __TS3_ENGINE_PNG_COMMON_H__
