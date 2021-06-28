
#pragma once

#ifndef __TS3_ENGINE_COMMON_FONT_DEFS_H__
#define __TS3_ENGINE_COMMON_FONT_DEFS_H__

#include "commonRDUtils.h"
#include "imageCommon.h"
#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/stdext/memoryBuffer.h>

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#  pragma clang diagnostic ignored "-Wnested-anon-types"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpedantic"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( push )
#  pragma warning( disable: 4201 )  // 'Nonstandard extension used: nameless struct/union'
#endif

namespace ts3
{

	struct FontCreateInfo;

	class Font;
	class FontLoader;
	class FontManager;

	using char_code_point_t = uint32;
	using char_kerning_value_t = int32;
	using font_index_t = uint32;
	using font_size_t = uint32;
	using font_type_t = uint64;
	using font_guid_t = uint64;
	using font_texture_index_t = uint32;

	using CharCodePointPair = std::pair<char_code_point_t, char_code_point_t>;
	using CharCodePointSet = std::set<char_code_point_t>;
	using FontHandle = std::shared_ptr<Font>;
	using FontLoaderPtr = std::unique_ptr<FontLoader>;
	using FontTexture = gpuapi::TextureHandle;

	/// @brief Kerning definition for a pair of characters.
	///
	/// Kerning is an additional modifier applied to spacing between two given characters (think of 'AV', 'LT' or 'ML').
	/// It is defined by a two code points to which the kerning is applied and a kerning value itself. Note, that the
	/// order of characters is important - kerning for 'LT' is likely to be negative, while 'TL' may have it positive.
	struct CharKerningInfo
	{
		CharCodePointPair codePointPair;
		char_kerning_value_t kerningValue;
	};

	/// @brief Represents a font glyph - the smallest logical unit we operate on, with its own metrics and (optional) image.
	///
	/// Glyph is the most basic element in a font engine. Each character (represented by a Unicode code point)
	/// can be consisted of one or more glyphs (however, the support for multi-glyph characters is not tested).
	struct FontGlyph
	{
		// Image reference. Specifies a sub-rectangle in a texture this glyph resides.
		// Note, that even though most font engines (BMF, FreeType) use font atlases,
		// it is possible, that this struct can reference an individual, small image
		// with only a single glyph in it.
		struct ImageRef
		{
			// Index of a texture. This is font-specific and not necessarily a texture
			// array index. Font engines may alternatively use few separate 2D textures
			// instead of a Texture2DArray.
			uint32 imageIndex;

			// Coordinates of a texture sub-rectangle which contains glyph's image.
			// Stored as normalized values in range [0.0, 1.0].
			math::Rectf rect;
		};

		// Glyph metrics. A rather self-descriptive thing.
		struct Metrics
		{
			math::Vec2u32 advance;
			math::Vec2u32 bearing;
			math::Vec2u32 dimensions;
		};

		char_code_point_t codePoint;
		ImageRef imageRef;
		Metrics metrics;

		// Additional, font engine-specific data. One of the common usages is to use it
		// for storing the image/texture if no atlas textures are used by the font engine.
		void * fontEngineData = nullptr;
	};

	/// @brief Represents a bitmap containing a font image. Used primarily for glyph images and initialized during glyph loading.
	///
	/// This struct is intended to be used as "source data", i.e. filled on glyph loading and copied to a target texture.
	/// Most fonts don't expose this struct at all and use it as a temporary storage. Some fonts, however, may keep it
	/// if "big" textures are not used. This is an implementation detail, though.
	struct FontImageData
	{
		// Pointer to the bitmap pixel array.
		const byte * data = nullptr;

		// Total size of the data, in bytes.
		size_t dataSize = 0;

		// The size of a single row of data, including any padding. May be larger than SizeOfPixel * BitmapWidth.
		size_t dataRowPitch;

		union
		{
			struct
			{
				uint32 width;
				uint32 height;
			};
			math::Vec2u32 dimensions;
		};

		explicit operator bool() const
		{
			return data && ( dataSize > 0 );
		}
	};

	struct CharKerningInfoCmpEqual
	{
		constexpr bool operator()( const CharKerningInfo & pLhs, const CharKerningInfo & pRhs ) const
		{
			return pLhs.codePointPair == pRhs.codePointPair;
		}

		constexpr bool operator()( const CharCodePointPair & pLhs, const CharKerningInfo & pRhs ) const
		{
			return pLhs == pRhs.codePointPair;
		}

		constexpr bool operator()( const CharKerningInfo & pLhs, const CharCodePointPair & pRhs ) const
		{
			return pLhs.codePointPair == pRhs;
		}
	};

	struct CharKerningInfoCmpLess
	{
		constexpr bool operator()( const CharKerningInfo & pLhs, const CharKerningInfo & pRhs ) const
		{
			return ( pLhs.codePointPair.first < pRhs.codePointPair.first ) ||
			       ( ( pLhs.codePointPair.first == pRhs.codePointPair.first ) && ( pLhs.codePointPair.second < pRhs.codePointPair.second ) );
		}

		constexpr bool operator()( const CharCodePointPair & pLhs, const CharKerningInfo & pRhs ) const
		{
			return ( pLhs.first < pRhs.codePointPair.first ) ||
			       ( ( pLhs.first == pRhs.codePointPair.first ) && ( pLhs.second < pRhs.codePointPair.second ) );
		}

		constexpr bool operator()( const CharKerningInfo & pLhs, const CharCodePointPair & pRhs ) const
		{
			return ( pLhs.codePointPair.first < pRhs.first ) ||
			       ( ( pLhs.codePointPair.first == pRhs.first ) && ( pLhs.codePointPair.second < pRhs.second ) );
		}
	};

}

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __TS3_ENGINE_COMMON_FONT_DEFS_H__
