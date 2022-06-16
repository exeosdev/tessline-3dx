
#include "imageCommon.h"

namespace ts3
{

	void ImageData::reset()
	{
		sizeInBytes = 0;
		formatInfo.pixelLayout = EPixelDataLayout::Undefined;
	}

	void ImageData::release()
	{
		reset();
		pixelBuffer.release();
	}

	ImageData ImageData::clone() const
	{
		ImageData imageDataClone;
		imageDataClone.pixelBuffer.resize( pixelBuffer.size() );
		imageDataClone.formatInfo = formatInfo;
		imageDataClone.sizeInBytes = sizeInBytes;

		memCopy( imageDataClone.pixelBuffer.data(),
		         imageDataClone.pixelBuffer.size(),
		         pixelBuffer.data(),
		         pixelBuffer.size() );

		return imageDataClone;
	}

	bool ImageData::empty() const
	{
		return pixelBuffer.empty() || ( sizeInBytes == 0 );
	}

	void ImageData::swap( ImageData & pOther )
	{
		std::swap( formatInfo, pOther.formatInfo );
		std::swap( pixelBuffer, pOther.pixelBuffer );
		std::swap( sizeInBytes, pOther.sizeInBytes );
	}

} // namespace ts3
