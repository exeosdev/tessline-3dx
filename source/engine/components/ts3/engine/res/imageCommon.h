
#ifndef __TS3_ENGINE_IMAGE_COMMON_H__
#define __TS3_ENGINE_IMAGE_COMMON_H__

#include "resourceCommon.h"
#include <ts3/core/graphicsTypes.h>
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{

	using ImageDataBuffer = DynamicMemoryBuffer;

	struct ImageFormatInfo
	{
		math::Vec2u32 dimensions;
		EPixelDataLayout pixelLayout;
		uint16 bitDepth = 0;
		uint16 pixelByteSize = 0;
	};

	struct ImageData
	{
	public:
		ImageFormatInfo formatInfo;
		ImageDataBuffer pixelBuffer;
		size_t sizeInBytes = 0;

	public:
		ImageData( const ImageData & ) = delete;
		ImageData & operator=( const ImageData & ) = delete;

		ImageData() = default;
		ImageData( ImageData && ) = default;
		ImageData & operator=( ImageData && ) = default;

		ImageData( std::nullptr_t )
		: ImageData()
		{}

		explicit operator bool() const
		{
			return !isEmpty() && ( formatInfo.pixelLayout != EPixelDataLayout::Undefined );
		}

		void reset();

		void release();

		ImageData clone() const;

		bool isEmpty() const;

		void swap( ImageData & pOther );
	};

}

#endif // __TS3_ENGINE_IMAGE_COMMON_H__
