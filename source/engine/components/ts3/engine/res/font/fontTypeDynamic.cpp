
#include "fontTypeDynamic.h"
#include "ftdFreeTypeFontFace.h"
#include "ftdFreeTypeFontObject.h"
#include <ts3/core/graphics/rectAllocator.h>
#include <unordered_map>

namespace ts3
{

	struct DynamicFontLoader::FTInternalFontData
	{
		struct LoadedGlyphData
		{
			FontGlyph glyph;
			FontImageData glyphImage;
		};

		using TextureLayerInitData = FontTextureCreateInfo::TextureLayerInitData;

		std::vector<LoadedGlyphData> glyphArray;
		std::vector<CharKerningInfo> charKerningArray;
		std::vector<TextureLayerInitData> textureLayerInitDataArray;
		std::unordered_map<char_code_point_t, FontGlyph> glyphMap;

		explicit operator bool() const
		{
			return !glyphArray.empty();
		}
	};

	struct DynamicFontLoader::FTFontLoaderPrivateData
	{
		using FreeTypeFontObjectMap = std::unordered_map<std::string, FreeTypeFontObjectPtr>;
		FreeTypeFontObjectMap freeTypeFontObjectMap;
	};

	FontHandle DynamicFontLoader::createFont( const DynamicFontCreateInfo & pFontCreateInfo )
	{
		auto * ftFontObject = createFreeTypeFontObject( pFontCreateInfo );
		if( !ftFontObject || !ftFontObject->setActiveFace( pFontCreateInfo.fontDesc.fontSize, pFontCreateInfo.fontResolutionHint ) )
		{
			return nullptr;
		}

		FTInternalFontData internalFontData;

		if( !loadInternalFontData( ftFontObject, pFontCreateInfo, internalFontData ) )
		{
			return nullptr;
		}

		if( !processFontInitData( ftFontObject, pFontCreateInfo, internalFontData ) )
		{
			return nullptr;
		}

		ftFontObject->resetActiveGlyphCache();

		return nullptr;
	}

	FreeTypeFontObject * DynamicFontLoader::createFreeTypeFontObject( const DynamicFontCreateInfo & pFontCreateInfo )
	{
		auto & ftFOntObjectMap = _privateData->freeTypeFontObjectMap;

		auto ftFontObjectIter = ftFOntObjectMap.find( pFontCreateInfo.fontDesc.fontID );
		if( ftFontObjectIter == ftFOntObjectMap.end() )
		{
			auto ftFontObject = FreeTypeFontObject::createFromMemory( pFontCreateInfo.binaryFontData.data(), pFontCreateInfo.binaryFontData.size() );
			if( ftFontObject )
			{
				auto insertIter = ftFOntObjectMap.insert( { pFontCreateInfo.fontDesc.fontID, std::move( ftFontObject ) } );
				ftFontObjectIter = insertIter.first;
			}
		}

		if( ftFontObjectIter != ftFOntObjectMap.end() )
		{
			return ftFontObjectIter->second.get();
		}

		return nullptr;
	}

	bool DynamicFontLoader::loadInternalFontData( FreeTypeFontObject * pFTFontObject, const DynamicFontCreateInfo & pFontCreateInfo, FTInternalFontData & pFontData )
	{
		FTInternalFontData fontData;
		fontData.glyphArray.reserve( pFontCreateInfo.preloadGlyphSet.size() );
		fontData.charKerningArray.reserve( pFontCreateInfo.preloadGlyphSet.size() );

		for( auto codePoint : pFontCreateInfo.preloadGlyphSet )
		{
			FTInternalFontData::LoadedGlyphData glyphData;
			if( pFTFontObject->loadGlyph( codePoint, &( glyphData.glyph ), &( glyphData.glyphImage ) ) )
			{
				pFontData.glyphArray.push_back( glyphData );
			}
		}

		if( !fontData.glyphArray.empty() )
		{
			return false;
		}

		for( auto codePoint1 : pFontCreateInfo.preloadGlyphSet )
		{
			for( auto codePoint2 : pFontCreateInfo.preloadGlyphSet )
			{
				CharKerningInfo kerningInfo{ { codePoint1, codePoint2 }, 0 };
				if( pFTFontObject->loadKerning( kerningInfo.codePointPair, &( kerningInfo.kerningValue ) ) )
				{
					pFontData.charKerningArray.push_back( kerningInfo );
				}
			}
		}

		if( !fontData.charKerningArray.empty() )
		{
			fontData.charKerningArray.shrink_to_fit();
		}

		pFontData = std::move( fontData );

		return true;
	}

	bool DynamicFontLoader::processFontInitData( FreeTypeFontObject * pFTFontObject, const DynamicFontCreateInfo & pFontCreateInfo, FTInternalFontData & pFontData )
	{
		// Size, in bytes, of a single pixel of a texture/glyph image.
		const uint32 cxPixelByteSize = gpuapi::cxdefs::getTextureFormatByteSize( pFontCreateInfo.fontDesc.textureFormat );

		const auto cxTextureWidth = pFontCreateInfo.fontDesc.textureDimensions.x;  // Width of a single font texture, in pixels.
		const auto cxTextureHeight = pFontCreateInfo.fontDesc.textureDimensions.y; // Height of a single font texture, in pixels.
		const auto cxTextureRowPitch = cxTextureWidth * cxPixelByteSize;  // Size, in bytes, of a single pixel row of a texture.
		const auto cxTextureSize = cxTextureHeight * cxTextureRowPitch;   // Size, in bytes, of a whole font texture.

		RectAllocatorConfig textureLayerAllocatorConfig;
		textureLayerAllocatorConfig.horizontalLayout.hSpacing = 2;
		textureLayerAllocatorConfig.verticalLayout.vSpacing = 2;
		textureLayerAllocatorConfig.verticalLayout.baseLineHeight = pFTFontObject->getLineHeight();

		// Allocator for texture space. Allocates rectangular subregions of a bounding rectangle.
		RectAllocator textureLayerAllocator{ pFontCreateInfo.fontDesc.textureDimensions, textureLayerAllocatorConfig };

		// Pointer to the current layer we are inserting glyphs into. When a layer is full (i.e. the allocator
		// fails to insert a new rect), we create a new one and reset the allocator to keep the process going.
		auto * currentTextureLayer = static_cast<FTInternalFontData::TextureLayerInitData *>( nullptr );

		for( auto & ftGlyphData : pFontData.glyphArray )
		{
			if( ftGlyphData.glyphImage )
			{
				// ImageRef - describes where the glyph's image is located within its texture font.
				auto & glyphImageRef = ftGlyphData.glyph.imageRef;

				// Glyph image - the image itself (pointer to the data, information about size, row pitch, etc).
				auto & glyphSourceImage = ftGlyphData.glyphImage;

				if( !currentTextureLayer || !textureLayerAllocator.checkFreeSpace( glyphSourceImage.dimensions ) )
				{
					// No space left in the current layer or no layer at all. Append a new layer...
					pFontData.textureLayerInitDataArray.emplace_back();
					// ...and reset the allocator.
					textureLayerAllocator.reset();

					currentTextureLayer = &( pFontData.textureLayerInitDataArray.back() );
					currentTextureLayer->layerIndex = numeric_cast<uint32>( pFontData.textureLayerInitDataArray.size() ) - 1;
					currentTextureLayer->initDataBuffer.resize( cxTextureSize );
					currentTextureLayer->initDataBuffer.fill( 0 );
				}

				math::Vec2u32 glyphImageTextureOffset;
				// This should never fail, because we already checked the available space (checkFreeSpace() above).
				// If this assertion triggers, it means, that the current glyph is bigger than the texture rect itself.
				if( !textureLayerAllocator.addRect( glyphSourceImage.dimensions, &glyphImageTextureOffset ) )
				{
					return false;
				}

				// The offset of a top-left corner of a target rectangle allocated for the current glyph in the target texture.
				const auto baseTargetPixelOffset = glyphImageTextureOffset.y * cxTextureWidth + glyphImageTextureOffset.x;
				// Size, in bytes, of single row of a glyph's image.
				const auto srcDataRowSize = glyphSourceImage.width * cxPixelByteSize;

				for( uint32 imageRowIndex = 0; imageRowIndex < glyphSourceImage.height; ++imageRowIndex )
				{
					const auto srcByteOffset = imageRowIndex * glyphSourceImage.dataRowPitch;
					const auto targetByteOffset = ( baseTargetPixelOffset + ( imageRowIndex * cxTextureWidth ) ) * cxPixelByteSize;
					auto * targetBufferBtr = currentTextureLayer->initDataBuffer.dataOffset( targetByteOffset );
					memCopy( targetBufferBtr, cxTextureSize, glyphSourceImage.data + srcByteOffset, srcDataRowSize );
				}

				glyphImageRef.imageIndex = currentTextureLayer->layerIndex;
				glyphImageRef.rect.offset.x = static_cast<float>( glyphImageTextureOffset.x ) / cxTextureWidth;
				glyphImageRef.rect.offset.y = static_cast<float>( glyphImageTextureOffset.y ) / cxTextureHeight;
				glyphImageRef.rect.size.x = static_cast<float>( glyphSourceImage.dimensions.x ) / cxTextureWidth;
				glyphImageRef.rect.size.y = static_cast<float>( glyphSourceImage.dimensions.y ) / cxTextureHeight;
			}

			pFontData.glyphMap[ftGlyphData.glyph.codePoint] = ftGlyphData.glyph;
		}

		return true;
	}

	FontTextureCreateInfo DynamicFontLoader::setupTextureCreateInfo( const DynamicFontCreateInfo & pFontCreateInfo, FTInternalFontData & pFontData )
	{
		const auto cxStaticSubTexturesNum = numeric_cast<uint32>( pFontData.textureLayerInitDataArray.size() );
		const auto cxDynamicSubTexturesNum = pFontCreateInfo.fontDesc.dynamicLayersNum;
		const auto cxTotalSubTexturesNum = cxStaticSubTexturesNum + cxDynamicSubTexturesNum;

		FontTextureCreateInfo textureCreateInfo;
		textureCreateInfo.textureLayerInitDataArray = std::move( pFontData.textureLayerInitDataArray );
		textureCreateInfo.gpuTextureCreateInfo.dimensions.depth = 1;
		textureCreateInfo.gpuTextureCreateInfo.dimensions.mipLevelsNum = 1;
		textureCreateInfo.gpuTextureCreateInfo.dimensions.width = pFontCreateInfo.fontDesc.textureDimensions.x;
		textureCreateInfo.gpuTextureCreateInfo.dimensions.height = pFontCreateInfo.fontDesc.textureDimensions.y;
		textureCreateInfo.gpuTextureCreateInfo.memoryBaseAlignment = 256;
		textureCreateInfo.gpuTextureCreateInfo.msaaLevel = 0;
		textureCreateInfo.gpuTextureCreateInfo.internalFormat = pFontCreateInfo.fontDesc.textureFormat;

		if( cxDynamicSubTexturesNum == 0 )
		{
			textureCreateInfo.gpuTextureCreateInfo.memoryFlags = gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
			textureCreateInfo.gpuTextureCreateInfo.resourceFlags = gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
		}
		else
		{
			textureCreateInfo.gpuTextureCreateInfo.memoryFlags = gpuapi::E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT | gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
			textureCreateInfo.gpuTextureCreateInfo.resourceFlags = gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
		}

		if( cxTotalSubTexturesNum == 1 )
		{
			textureCreateInfo.gpuTextureCreateInfo.texClass = gpuapi::ETextureClass::T2D;
			textureCreateInfo.gpuTextureCreateInfo.dimensions.arraySize = 1;
		}
		else
		{
			textureCreateInfo.gpuTextureCreateInfo.texClass = gpuapi::ETextureClass::T2DArray;
			textureCreateInfo.gpuTextureCreateInfo.dimensions.arraySize = cxTotalSubTexturesNum;
		}

		auto & gpuTextureInitDataDesc = textureCreateInfo.gpuTextureCreateInfo.initDataDesc;
		gpuTextureInitDataDesc.initialize( textureCreateInfo.gpuTextureCreateInfo.dimensions );

		for( uint32 staticSubTextureIndex = 0; staticSubTextureIndex < cxStaticSubTexturesNum; ++staticSubTextureIndex )
		{
			const auto & textureLayerDataDesc = textureCreateInfo.textureLayerInitDataArray[staticSubTextureIndex];

			// Each font sub-texture has only one mip level, as mip-mapping is disabled for fonts.
			auto & subTextureInitData = gpuTextureInitDataDesc.subTextureInitDataBasePtr[staticSubTextureIndex].mipLevelInitDataArray[0];
			subTextureInitData.pointer = textureLayerDataDesc.initDataBuffer.data();
			subTextureInitData.size = textureLayerDataDesc.initDataBuffer.size();
		}

		for( uint32 dynamicSubTextureBaseIndex = 0; dynamicSubTextureBaseIndex < cxDynamicSubTexturesNum; ++dynamicSubTextureBaseIndex )
		{
			// Dynamic font layers come directly after the static/pre-rendered ones.
			uint32 dynamicSubTextureIndex = cxStaticSubTexturesNum + dynamicSubTextureBaseIndex;

			auto & subTextureInitData = gpuTextureInitDataDesc.subTextureInitDataBasePtr[dynamicSubTextureIndex].mipLevelInitDataArray[0];
			subTextureInitData.pointer = nullptr;
			subTextureInitData.size = 0;
		}

		return textureCreateInfo;
	}

} // namespace ts3

