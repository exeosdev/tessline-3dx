
#pragma once

#ifndef __TS3_ENGINE_FONT_TEXTURE_H__
#define __TS3_ENGINE_FONT_TEXTURE_H__

#include "fontCommon.h"

namespace ts3
{

	/// @brief Data of a texture object used by a font to store glyph images.
	struct FontTextureInfo
	{
		FontTextureInfo() = default;

		FontTextureInfo( std::nullptr_t )
		: texture( nullptr )
		, textureArraySize( 0 )
		{}

		gpuapi::TextureHandle texture;

		// Texture dimension. Usually - a single 2D texture or an array of those.
		gpuapi::ETextureDimensionClass dimensionClass;

		// Internal format of the texture.
		gpuapi::ETextureFormat format;

		// Number of sub-textures (layers) in the texture. 1 for a single texture,
		// 1 or more for texture arrays.
		uint32 textureArraySize;

		explicit operator bool() const
		{
			return texture && ( textureArraySize > 0 );
		}
	};

	struct FontTextureSubDataDesc
	{
		const byte * data = nullptr;
		size_t dataSize;
		size_t dataRowPitch;
		uint32 subTextureIndex;
		math::Vec2u32 targetOffset;
		math::Vec2u32 targetRect;

		explicit operator bool() const
		{
			return data != nullptr;
		}
	};

	class FontTexturePreloadCache
	{
	public:
		using PreloadedSubDataArray = std::vector<FontTextureSubDataDesc>;

		FontTexturePreloadCache() = default;

		FontTexturePreloadCache( FontTexturePreloadCache && ) = default;
		FontTexturePreloadCache & operator=( FontTexturePreloadCache && ) = default;

		byte * allocateCacheMemory( size_t pMemorySize );

		bool resize( size_t pCacheCapacity );

		void reset();

		const PreloadedSubDataArray & getPreloadedSubData() const;

		bool isEmpty() const;

		bool isFull() const;

	private:
		size_t _dataBufferCapacity;
		size_t _dataBufferAllocOffset;
		ts3::DynamicMemoryBuffer _dataBuffer;
		PreloadedSubDataArray _preloadedSubData;
	};

}

#endif // __TS3_ENGINE_FONT_TEXTURE_H__
