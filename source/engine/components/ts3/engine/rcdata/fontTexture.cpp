
#include "fontTexture.h"

namespace ts3
{

	byte * FontTexturePreloadCache::allocateCacheMemory( size_t pMemorySize )
	{
		if( _dataBufferAllocOffset + pMemorySize > _dataBufferCapacity )
		{
			return nullptr;
		}

		auto * allocPtr = _dataBuffer.dataPtr() + _dataBufferAllocOffset;
		_dataBufferAllocOffset += pMemorySize;

		return allocPtr;
	}

	bool FontTexturePreloadCache::resize( size_t pCacheCapacity )
	{
		return false;
	}

	void FontTexturePreloadCache::reset()
	{
		_dataBufferAllocOffset = 0;
	}

	const FontTexturePreloadCache::PreloadedSubDataArray & FontTexturePreloadCache::getPreloadedSubData() const
	{
		return _preloadedSubData;
	}

	bool FontTexturePreloadCache::isEmpty() const
	{
		return _dataBufferAllocOffset == 0;
	}

	bool FontTexturePreloadCache::isFull() const
	{
		return _dataBufferAllocOffset == _dataBufferCapacity;
	}

}
