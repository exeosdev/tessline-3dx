
#include "texture.h"

namespace ts3::gpuapi
{

	ETextureTarget getETextureTargetFromResourceFlags( const Bitmask<resource_flags_value_t> & pTextureResourceFlags );

	Texture::Texture(
			GPUDevice & pGPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const TextureProperties & pTextureProperties,
			const TextureLayout & pTextureLayout )
	: GPUResource( pGPUDevice, EGPUResourceBaseType::Texture, pResourceMemory )
	, mTextureProperties( pTextureProperties )
	, mTextureLayout( pTextureLayout )
	{}

	Texture::~Texture() = default;

	const GPUResourceProperties & Texture::getProperties() const
	{
		return mTextureProperties;
	}

	bool Texture::checkTextureTargetSupport( ETextureTarget pTextureTarget ) const
	{
		return true;
	}

	TextureSubResource Texture::getAllSubResourcesRef() const
	{
		switch( mTextureLayout.texClass )
		{
			case ETextureClass::T2D:
			{
				TextureSubResource2D subResource2D;
				subResource2D.mipLevel = cxdefs::TEX_SUBRESOURCE_MIP_LEVEL_ALL_MIPS;
				return TextureSubResource{ subResource2D, ETextureClass::T2D };
			}
			case ETextureClass::T2DArray:
			{
				TextureSubResource2DArray subResource2DArray;
				subResource2DArray.arrayIndex = cxdefs::TEX_SUBRESOURCE_ARRAY_INDEX_ALL_TEXTURES;
				subResource2DArray.mipLevel = cxdefs::TEX_SUBRESOURCE_MIP_LEVEL_ALL_MIPS;
				return TextureSubResource{ subResource2DArray, ETextureClass::T2DArray };
			}
			case ETextureClass::T2DMS:
			{
				TextureSubResource2D subResource2DMS;
				subResource2DMS.mipLevel = 0; // MSAA textures only have a single sub-resource (mip level 0).
				return TextureSubResource{ subResource2DMS, ETextureClass::T2DMS };
			}
			case ETextureClass::T2DMSArray:
			{
				TextureSubResource2DArray subResource2DMSArray;
				subResource2DMSArray.arrayIndex = cxdefs::TEX_SUBRESOURCE_ARRAY_INDEX_ALL_TEXTURES;
				subResource2DMSArray.mipLevel = 0; // MSAA textures only have a single sub-resource (mip level 0).
				return TextureSubResource{ subResource2DMSArray, ETextureClass::T2DMSArray };
			}
			case ETextureClass::T3D:
			{
				TextureSubResource3D subResource3D;
				subResource3D.mipLevel = cxdefs::TEX_SUBRESOURCE_MIP_LEVEL_ALL_MIPS;
				subResource3D.depthLayerIndex = cxdefs::TEX_SUBRESOURCE_DEPTH_ALL_LAYERS;
				return TextureSubResource{ subResource3D };
			}
			case ETextureClass::TCubeMap:
			{
				TextureSubResourceCubeMap subResourceCubeMap;
				subResourceCubeMap.faceIndex = cxdefs::TEX_SUBRESOURCE_CUBE_MAP_FACE_ALL_FACES;
				subResourceCubeMap.mipLevel = cxdefs::TEX_SUBRESOURCE_MIP_LEVEL_ALL_MIPS;
				return TextureSubResource{ subResourceCubeMap };
			}
			default:
			{
				return TextureSubResource{};
			}
		}
	}

	TextureSubResource Texture::getDefaultSubResourceRef() const
	{
		switch( mTextureLayout.texClass )
		{
			case ETextureClass::T2D:
			{
				TextureSubResource2D subResource2D;
				subResource2D.mipLevel = 0;
				return TextureSubResource{ subResource2D, ETextureClass::T2D };
			}
			case ETextureClass::T2DArray:
			{
				TextureSubResource2DArray subResource2DArray;
				subResource2DArray.arrayIndex = 0;
				subResource2DArray.mipLevel = 0;
				return TextureSubResource{ subResource2DArray, ETextureClass::T2DArray };
			}
			case ETextureClass::T2DMS:
			{
				TextureSubResource2D subResource2DMS;
				subResource2DMS.mipLevel = 0;
				return TextureSubResource{ subResource2DMS, ETextureClass::T2DMS };
			}
			case ETextureClass::T2DMSArray:
			{
				TextureSubResource2DArray subResource2DMSArray;
				subResource2DMSArray.arrayIndex = 0;
				subResource2DMSArray.mipLevel = 0;
				return TextureSubResource{ subResource2DMSArray, ETextureClass::T2DMSArray };
			}
			case ETextureClass::T3D:
			{
				TextureSubResource3D subResource3D;
				subResource3D.mipLevel = 0;
				subResource3D.depthLayerIndex = cxdefs::TEX_SUBRESOURCE_DEPTH_ALL_LAYERS;
				return TextureSubResource{ subResource3D };
			}
			case ETextureClass::TCubeMap:
			{
				TextureSubResourceCubeMap subResourceCubeMap;
				subResourceCubeMap.faceIndex = E_TEXTURE_CUBE_MAP_FACE_FIRST;
				subResourceCubeMap.mipLevel = 0;
				return TextureSubResource{ subResourceCubeMap };
			}
			default:
			{
				return TextureSubResource{};
			}
		}
	}

	bool Texture::validateTextureCreateInfo( TextureCreateInfo & pCreateInfo )
	{
		if( pCreateInfo.initialTarget != ETextureTarget::Unknown )
		{
			// If the user has specified explicit initial target for the buffer, we need to verify
			// if the buffer description allows for that target in the first place.
			if( !pCreateInfo.resourceFlags.isSet( pCreateInfo.initialTarget ) )
			{
				pCreateInfo.resourceFlags.set( pCreateInfo.initialTarget );
			}
		}
		else
		{
			auto initialTarget = getETextureTargetFromResourceFlags( pCreateInfo.resourceFlags );
			if( initialTarget == ETextureTarget::Unknown )
			{
				return false;
			}
			pCreateInfo.initialTarget = initialTarget;
		}

		if( pCreateInfo.memoryBaseAlignment == 0 )
		{
		    pCreateInfo.memoryBaseAlignment = ts3::cxdefs::MEMORY_DEFAULT_ALIGNMENT;
		}

		if( pCreateInfo.texClass == ETextureClass::T2D )
		{
			pCreateInfo.dimensions.arraySize = 1;
			pCreateInfo.dimensions.depth = 1;
		}
		else if( pCreateInfo.texClass == ETextureClass::T2DArray )
		{
			pCreateInfo.dimensions.depth = 1;
		}
		else if( pCreateInfo.texClass == ETextureClass::T2DMS )
		{
			pCreateInfo.dimensions.arraySize = 1;
			pCreateInfo.dimensions.depth = 1;
			pCreateInfo.dimensions.mipLevelsNum = 1;
		}
		else if( pCreateInfo.texClass == ETextureClass::T2DMSArray )
		{
			pCreateInfo.dimensions.depth = 1;
			pCreateInfo.dimensions.mipLevelsNum = 1;
		}
		else if( pCreateInfo.texClass == ETextureClass::T3D )
		{
			pCreateInfo.dimensions.arraySize = 1;
		}
		else if( pCreateInfo.texClass == ETextureClass::TCubeMap )
		{
			pCreateInfo.dimensions.depth = 1;
			pCreateInfo.dimensions.arraySize = 1;
		}

		return true;
	}


	ETextureTarget getETextureTargetFromResourceFlags( const Bitmask<resource_flags_value_t> & pTextureResourceFlags )
	{
		static const ETextureTarget textureTargetArray[] =
		{
			ETextureTarget::RenderTargetColorAttachment,
			ETextureTarget::RenderTargetDepthStencilAttachment,
			ETextureTarget::ShaderInputSampledImage,
			ETextureTarget::TransferSourceImage,
			ETextureTarget::TransferSourceImage,
		};

		for( auto textureTarget : textureTargetArray )
		{
			auto textureTargetResourceFlags = static_cast<resource_flags_value_t>( textureTarget );
			if( pTextureResourceFlags.isSet( textureTargetResourceFlags ) )
			{
				return textureTarget;
			}
		}

		return ETextureTarget::Unknown;
	}

} // namespace ts3::gpuapi
