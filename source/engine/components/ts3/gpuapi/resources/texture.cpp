
#include "texture.h"

namespace ts3
{
namespace gpuapi
{

	ETextureTarget getETextureTargetFromResourceFlags( const Bitmask<resource_flags_value_t> & pTextureResourceFlags );

	Texture::Texture( GPUDevice & pGPUDevice,
	                  const ResourceMemoryInfo & pResourceMemory,
	                  const TextureProperties & pTextureProperties,
	                  const TextureLayout & pTextureLayout )
	: GPUResource( pGPUDevice, EGPUResourceBaseType::Texture, pResourceMemory )
	, mTextureProperties( pTextureProperties )
	, mTextureLayout( pTextureLayout )
	{}

	Texture::~Texture() = default;

	bool Texture::checkETextureTargetSupport( ETextureTarget pTextureTarget ) const
	{
		return true;
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
		    pCreateInfo.memoryBaseAlignment = CX_MEMORY_DEFAULT_ALIGNMENT;
		}

		if( pCreateInfo.dimensionClass == ETextureDimensionClass::Texture2D )
		{
			pCreateInfo.dimensions.arraySize = 1;
			pCreateInfo.dimensions.depth = 1;
		}
		else if( pCreateInfo.dimensionClass == ETextureDimensionClass::Texture2DArray )
		{
			pCreateInfo.dimensions.depth = 1;
		}
		else if( pCreateInfo.dimensionClass == ETextureDimensionClass::Texture2DMS )
		{
			pCreateInfo.dimensions.arraySize = 1;
			pCreateInfo.dimensions.depth = 1;
			pCreateInfo.dimensions.mipLevelsNum = 1;
		}
		else if( pCreateInfo.dimensionClass == ETextureDimensionClass::Texture2DMSArray )
		{
			pCreateInfo.dimensions.depth = 1;
			pCreateInfo.dimensions.mipLevelsNum = 1;
		}
		else if( pCreateInfo.dimensionClass == ETextureDimensionClass::Texture3D )
		{
			pCreateInfo.dimensions.arraySize = 1;
		}
		else if( pCreateInfo.dimensionClass == ETextureDimensionClass::TextureCubeMap )
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

} /* namespace ts3 */
} /* namespace gpuapi */
