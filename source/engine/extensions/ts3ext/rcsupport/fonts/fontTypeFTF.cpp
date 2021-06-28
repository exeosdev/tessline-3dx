
#include "fontTypeFTF.h"

namespace ts3
{

	FreeTypeFont::FreeTypeFont( font_guid_t pFontGUID,
	                            font_size_t pFontSize,
	                            const FontTextureInfo & pFontTextureInfo )
    : Font( pFontGUID, pFontSize, pFontTextureInfo, std::make_unique<FontTexturePreloadCache>() )
	{}

	FreeTypeFont::~FreeTypeFont() = default;


	FreeTypeFontLoader::FreeTypeFontLoader( gpuapi::CommandContext & pGPUCommandContext )
	: FontLoader( pGPUCommandContext )
	{}

	FontHandle FreeTypeFontLoader::createFont( const FontCreateInfo & pFontCreateInfo )
	{
		const auto * freeTypeCreateInfo = reinterpret_cast<const FreeTypeFontCreateInfo *>( &pFontCreateInfo );

		auto * ftFontObject = createFreeTypeFontObject( *freeTypeCreateInfo );
		if( !ftFontObject || !ftFontObject->setActiveFace( freeTypeCreateInfo->fontSize, freeTypeCreateInfo->fontResolutionHint ) )
		{
			return nullptr;
		}

		auto ftFontData = loadFontData( ftFontObject, *freeTypeCreateInfo );
		if( !ftFontData )
		{
			return nullptr;
		}

		initializeTextureData( ftFontObject, ftFontData, *freeTypeCreateInfo );
		ftFontObject->resetActiveGlyphCache();

		auto gpuTextureCreateInfo = getTextureCreateInfo( ftFontData, *freeTypeCreateInfo );
		auto fontTextureInfo = FontLoader::createFontTexture( gpuTextureCreateInfo );

		if( !fontTextureInfo )
		{
			return nullptr;
		}

		auto fontObject = std::make_shared<FreeTypeFont>( pFontCreateInfo.fontGUID, pFontCreateInfo.fontSize, fontTextureInfo );
		fontObject->setGlyphMap( std::move( ftFontData.glyphMap ) );
		fontObject->setKerningInfoArray( std::move( ftFontData.charKerningArray ) );

		return fontObject;
	}

	FreeTypeFontData FreeTypeFontLoader::loadFontData( FreeTypeFontObject * pFTFontObject,
	                                                   const FreeTypeFontCreateInfo & pFontCreateInfo )
	{
		FreeTypeFontData ftFontData;
		ftFontData.glyphArray.reserve( pFontCreateInfo.preloadGlyphSet.size() );
		ftFontData.charKerningArray.reserve( pFontCreateInfo.preloadGlyphSet.size() );

		for( auto codePoint : pFontCreateInfo.preloadGlyphSet )
		{
			FreeTypeGlyphData freeTypeGlyph;
			if( pFTFontObject->loadGlyph( codePoint, &( freeTypeGlyph.glyph ), &( freeTypeGlyph.glyphImage ) ) )
			{
				ftFontData.glyphArray.push_back( freeTypeGlyph );
			}
		}

		for( auto codePoint1 : pFontCreateInfo.preloadGlyphSet )
		{
			for( auto codePoint2 : pFontCreateInfo.preloadGlyphSet )
			{
				CharKerningInfo kerningInfo{ { codePoint1, codePoint2 }, 0 };
				if( pFTFontObject->loadKerning( kerningInfo.codePointPair, &( kerningInfo.kerningValue ) ) )
				{
					ftFontData.charKerningArray.push_back( kerningInfo );
				}
			}
		}

		return ftFontData;
	}

	void FreeTypeFontLoader::initializeTextureData( FreeTypeFontObject * pFTFontObject,
	                                                FreeTypeFontData & pFTFontData,
	                                                const FreeTypeFontCreateInfo & pFontCreateInfo )
	{
		// Size, in bytes, of a single pixel of a texture/glyph image.
		const uint32 cxPixelByteSize = gpuapi::ecGetTextureFormatByteSize( pFontCreateInfo.textureFormat );

		const auto cxTextureWidth = pFontCreateInfo.textureDimensions.x;       // Width of a single font texture, in pixels.
		const auto cxTextureHeight = pFontCreateInfo.textureDimensions.y;      // Height of a single font texture, in pixels.
		const auto cxTextureRowPitch = cxTextureWidth * cxPixelByteSize; // Size, in bytes, of a single pixel row of a texture.
		const auto cxTextureSize = cxTextureHeight * cxTextureRowPitch;  // Size, in bytes, of a whole font texture.

		RectAllocatorConfig textureLayerAllocatorConfig;
		textureLayerAllocatorConfig.horizontalLayout.hSpacing = 2;
		textureLayerAllocatorConfig.verticalLayout.vSpacing = 2;
		textureLayerAllocatorConfig.verticalLayout.baseLineHeight = pFTFontObject->getLineHeight();

		// Allocator for texture space. Allocates rectangular subregions of a bounding rectangle.
		RectAllocator textureLayerAllocator{ pFontCreateInfo.textureDimensions, textureLayerAllocatorConfig };

		// Pointer to the current layer we are inserting glyphs into. When a layer is full (i.e. the allocator
		// fails to insert a new rect), we create a new one and reset the allocator to keep the process going.
		auto * currentTextureLayer = static_cast<FreeTypeFontData::TextureLayerData *>( nullptr );

		for( auto & ftGlyphData : pFTFontData.glyphArray )
		{
			if( ftGlyphData.glyphImage )
			{
				// ImageRef - where the glyph's image is located within its texture font.
				auto & glyphImageRef = ftGlyphData.glyph.imageRef;
				// Glyph image - the image itself (pointer to the data, information about size, row pitch, etc).
				auto & glyphSourceImage = ftGlyphData.glyphImage;

				if( !currentTextureLayer || !textureLayerAllocator.checkFreeSpace( glyphSourceImage.dimensions ) )
				{
					// No space left in the current layer or no layer at all. Append a new layer...
					pFTFontData.textureLayerDataArray.emplace_back();
					// ...and reset the allocator.
					textureLayerAllocator.reset();

					currentTextureLayer = &( pFTFontData.textureLayerDataArray.back() );
					currentTextureLayer->layerIndex = trunc_numeric_cast<uint32>( pFTFontData.textureLayerDataArray.size() ) - 1;
					currentTextureLayer->initDataBuffer.resize( cxTextureSize );
					currentTextureLayer->initDataBuffer.fillBytes( 0 );
				}

				math::Vec2u32 glyphImageTextureOffset;
				// This should never fail, because we already checked the available space (checkFreeSpace() above).
				// If this assertion triggers, it means, that the current glyph is bigger than the texture rect itself.
				bool addResult = textureLayerAllocator.addRect( glyphSourceImage.dimensions, &glyphImageTextureOffset );
				ts3DebugAssert( addResult );

				// The offset of a top-left corner of a target rectangle allocated for the current glyph in the target texture.
				const auto baseTargetPixelOffset = glyphImageTextureOffset.y * cxTextureWidth + glyphImageTextureOffset.x;
				// Size, in bytes, of single row of a glyph's image.
				const auto srcDataRowSize = glyphSourceImage.width * cxPixelByteSize;

				for( uint32 imageRowIndex = 0; imageRowIndex < glyphSourceImage.height; ++imageRowIndex )
				{
					const auto srcByteOffset = imageRowIndex * glyphSourceImage.dataRowPitch;
					const auto targetByteOffset = ( baseTargetPixelOffset + ( imageRowIndex * cxTextureWidth ) ) * cxPixelByteSize;
					auto * targetBufferBtr = currentTextureLayer->initDataBuffer.dataPtr() + targetByteOffset;
					memCopy( targetBufferBtr, cxTextureSize, glyphSourceImage.data + srcByteOffset, srcDataRowSize );
				}

				glyphImageRef.imageIndex = currentTextureLayer->layerIndex;
				glyphImageRef.rect.offset.x = static_cast<float>( glyphImageTextureOffset.x ) / cxTextureWidth;
				glyphImageRef.rect.offset.y = static_cast<float>( glyphImageTextureOffset.y ) / cxTextureHeight;
				glyphImageRef.rect.size.x = static_cast<float>( glyphSourceImage.dimensions.x ) / cxTextureWidth;
				glyphImageRef.rect.size.y = static_cast<float>( glyphSourceImage.dimensions.y ) / cxTextureHeight;
			}

			pFTFontData.glyphMap[ftGlyphData.glyph.codePoint] = ftGlyphData.glyph;
		}
	}

	FreeTypeFontObject * FreeTypeFontLoader::createFreeTypeFontObject( const FreeTypeFontCreateInfo & pFontCreateInfo )
	{
		auto ftFontInstanceIter = _ftFontInstanceMap.find( pFontCreateInfo.freeTypeFontID );
		if( ftFontInstanceIter == _ftFontInstanceMap.end() )
		{
			auto ftFontObject = loadResourceFromFile( pFontCreateInfo.freeTypeFontFile, createFreeTypeFontObjectFromMemory );
			if( ftFontObject )
			{
				FTFontInstance ftFontInstance;
				ftFontInstance.ftFontID = pFontCreateInfo.freeTypeFontID;
				ftFontInstance.ftFontFile = pFontCreateInfo.freeTypeFontFile;
				ftFontInstance.ftFontObject = std::move( ftFontObject );

				auto insertIter = _ftFontInstanceMap.insert( { pFontCreateInfo.freeTypeFontID, std::move( ftFontInstance ) } );
				ts3DebugAssert( insertIter.second );

				ftFontInstanceIter = insertIter.first;
			}
		}

		if( ftFontInstanceIter != _ftFontInstanceMap.end() )
		{
			return ftFontInstanceIter->second.ftFontObject.get();
		}

		return nullptr;
	}

    gpuapi::TextureCreateInfo FreeTypeFontLoader::getTextureCreateInfo( const FreeTypeFontData & pFTFontData,
                                                                        const FreeTypeFontCreateInfo & pFontCreateInfo )
    {
        const auto staticSubTexturesNum = trunc_numeric_cast<uint32>( pFTFontData.textureLayerDataArray.size() );
        const auto dynamicSubTexturesNum = pFontCreateInfo.dynamicLayersNum;
        const auto totalSubTexturesNum = staticSubTexturesNum + dynamicSubTexturesNum;

        gpuapi::TextureCreateInfo gpuTextureCreateInfo;
        gpuTextureCreateInfo.dimensions.depth = 1;
        gpuTextureCreateInfo.dimensions.mipLevelsNum = 1;
        gpuTextureCreateInfo.dimensions.width = pFontCreateInfo.textureDimensions.x;
        gpuTextureCreateInfo.dimensions.height = pFontCreateInfo.textureDimensions.y;
        gpuTextureCreateInfo.memoryBaseAlignment = 256;
        gpuTextureCreateInfo.msaaLevel = 0;
        gpuTextureCreateInfo.pixelFormat = pFontCreateInfo.textureFormat;
        gpuTextureCreateInfo.initialTarget = gpuapi::ETextureTarget::ShaderInputSampledImage;

        if( dynamicSubTexturesNum == 0 )
        {
            gpuTextureCreateInfo.memoryFlags = gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
            gpuTextureCreateInfo.resourceFlags = gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
        }
        else
        {
            gpuTextureCreateInfo.memoryFlags = gpuapi::E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT | gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
            gpuTextureCreateInfo.resourceFlags = gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT;
        }

        if( totalSubTexturesNum == 1 )
        {
            gpuTextureCreateInfo.dimensionClass = gpuapi::ETextureDimensionClass::Texture2D;
            gpuTextureCreateInfo.dimensions.arraySize = 1;
        }
        else
        {
            gpuTextureCreateInfo.dimensionClass = gpuapi::ETextureDimensionClass::Texture2DArray;
            gpuTextureCreateInfo.dimensions.arraySize = totalSubTexturesNum;
        }

        gpuTextureCreateInfo.initDataDesc.initialize( gpuTextureCreateInfo.dimensions );

        for( uint32 staticSubTextureIndex = 0; staticSubTextureIndex < staticSubTexturesNum; ++staticSubTextureIndex )
        {
            const auto & textureLayerDataDesc = pFTFontData.textureLayerDataArray[staticSubTextureIndex];

            // Each font sub-texture has only one mip level, as mip-mapping is disabled for fonts.
            auto & subTextureInitData = gpuTextureCreateInfo.initDataDesc.subTextureInitDataBasePtr[staticSubTextureIndex].mipLevelInitDataArray[0];
            subTextureInitData.pointer = textureLayerDataDesc.initDataBuffer.dataPtr();
            subTextureInitData.size = textureLayerDataDesc.initDataBuffer.size();
        }

        for( uint32 dynamicSubTextureBaseIndex = 0; dynamicSubTextureBaseIndex < dynamicSubTexturesNum; ++dynamicSubTextureBaseIndex )
        {
            // Dynamic font layers come directly after the static/pre-rendered ones.
            uint32 dynamicSubTextureIndex = staticSubTexturesNum + dynamicSubTextureBaseIndex;

            auto & subTextureInitData = gpuTextureCreateInfo.initDataDesc.subTextureInitDataBasePtr[dynamicSubTextureIndex].mipLevelInitDataArray[0];
            subTextureInitData.pointer = nullptr;
            subTextureInitData.size = 0;
        }

        return gpuTextureCreateInfo;
    }

}
