
#include "texture.h"
#include <ts3/stdext/utilities.h>

namespace ts3::gpuapi
{

	TextureInitDataDesc::TextureInitDataDesc( TextureInitDataDesc && pSrcObject )
	{
		if( !pSrcObject.empty() )
		{
			if( pSrcObject.isArray() )
			{
				_subTextureInitDataArray = std::move( pSrcObject._subTextureInitDataArray );
				subTextureInitDataBasePtr = _subTextureInitDataArray.data();
			}
			else
			{
				_subTextureInitData = pSrcObject._subTextureInitData;
				subTextureInitDataBasePtr = &_subTextureInitData;
			}
			pSrcObject.subTextureInitDataBasePtr = nullptr;
		}
	}

	TextureInitDataDesc & TextureInitDataDesc::operator=( TextureInitDataDesc && pRhs )
	{
		TextureInitDataDesc( std::move( pRhs ) ).swap( *this );
		return *this;
	}

	TextureInitDataDesc::TextureInitDataDesc( const TextureInitDataDesc & pInitData )
	{
		if( !pInitData.empty() )
		{
			if( pInitData.isArray() )
			{
				_subTextureInitDataArray = pInitData._subTextureInitDataArray;
				subTextureInitDataBasePtr = _subTextureInitDataArray.data();
			}
			else
			{
				_subTextureInitData = pInitData._subTextureInitData;
				subTextureInitDataBasePtr = &_subTextureInitData;
			}
		}
	}

	TextureInitDataDesc & TextureInitDataDesc::operator=( const TextureInitDataDesc & pRhs )
	{
		TextureInitDataDesc( pRhs ).swap( *this );
		return *this;
	}

	void TextureInitDataDesc::initialize( const TextureDimensions & pDimensions )
	{
		ts3DebugAssert( pDimensions.arraySize > 0 );
		ts3DebugAssert( pDimensions.mipLevelsNum > 0 );

		if( pDimensions.arraySize == 1 )
		{
			subTextureInitDataBasePtr = &_subTextureInitData;
		}
		else
		{
			_subTextureInitDataArray.resize( pDimensions.arraySize );
			subTextureInitDataBasePtr = _subTextureInitDataArray.data();
		}

		for( uint32 subTextureIndex = 0; subTextureIndex < pDimensions.arraySize; ++subTextureIndex )
		{
			auto & subTextureInitData = subTextureInitDataBasePtr[subTextureIndex];
			subTextureInitData.subTextureIndex = subTextureIndex;

			uint32 mipLevelWidth = pDimensions.width;
			uint32 mipLevelHeight = pDimensions.height;
			uint32 mipLevelDepth = pDimensions.depth;

			for( uint32 mipLevelIndex = 0; mipLevelIndex < pDimensions.mipLevelsNum; ++mipLevelIndex )
			{
				auto & mipLevelInitData = subTextureInitData.mipLevelInitDataArray[mipLevelIndex];
				mipLevelInitData.mipLevelIndex = mipLevelIndex;
				mipLevelInitData.mipWidth = mipLevelWidth;
				mipLevelInitData.mipHeight = mipLevelHeight;
				mipLevelInitData.mipDepth = mipLevelDepth;

				mipLevelWidth = getMaxOf( mipLevelWidth >> 1, 1u );
				mipLevelHeight = getMaxOf( mipLevelHeight >> 1, 1u );
				mipLevelDepth = getMaxOf( mipLevelDepth >> 1, 1u );
			}

			if( pDimensions.mipLevelsNum > 1 )
			{
				const auto & sMipLevel = subTextureInitData.mipLevelInitDataArray[pDimensions.mipLevelsNum - 2];
				ts3DebugAssert( ( sMipLevel.mipWidth != 1 ) || ( sMipLevel.mipHeight != 1 ) || ( sMipLevel.mipDepth != 1 ) );
			}
		}
	}

	void TextureInitDataDesc::swap( TextureInitDataDesc & pOther )
	{
		if( _subTextureInitDataArray.empty() )
		{
			std::swap( _subTextureInitData, pOther._subTextureInitData );
		}
		else
		{
			std::swap( _subTextureInitDataArray, pOther._subTextureInitDataArray );
			std::swap( subTextureInitDataBasePtr, pOther.subTextureInitDataBasePtr );
		}
	}

	bool TextureInitDataDesc::isArray() const
	{
		return !_subTextureInitDataArray.empty();
	}

	bool TextureInitDataDesc::empty() const
	{
		return subTextureInitDataBasePtr == nullptr;
	}

	TextureInitDataDesc TextureInitDataDesc::copy() const
	{
		TextureInitDataDesc copyResult;
		if( _subTextureInitDataArray.empty() )
		{
			copyResult._subTextureInitData = _subTextureInitData;
			copyResult.subTextureInitDataBasePtr = &( copyResult._subTextureInitData );
		}
		else
		{
			copyResult._subTextureInitDataArray = _subTextureInitDataArray;
			copyResult.subTextureInitDataBasePtr = copyResult._subTextureInitDataArray.data();
		}
		return copyResult;
	}


	namespace rcutil
	{

		static const TextureLayout sInvalidTextureLayout {
			ETextureClass::Unknown,
			TextureDimensions {
				0, 0, 0, 0, 0
			},
			0,
			0,
			0,
			ETextureFormat::UNKNOWN
		};

		bool checkRenderTargetTextureColorFormat( ETextureFormat pFormat ) noexcept
		{
			const Bitmask<uint8> pixelFormatFlags = cxdefs::getTextureFormatFlags( pFormat );

			// RT textures for color attachments can have any format except for compressed
			// formats and those specifically meant for depth/stencil attachments.
			return !pixelFormatFlags.isSetAnyOf( E_GPU_DATA_FORMAT_FLAG_DEPTH_STENCIL_BIT | E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT );
		}

		bool checkRenderTargetTextureDepthStencilFormat( ETextureFormat pFormat ) noexcept
		{
			const Bitmask<uint8> pixelFormatFlags = cxdefs::getTextureFormatFlags( pFormat );

			// Only depth and combined depth/stencil formats are valid for depth/stencil textures.
			return pixelFormatFlags.isSetAnyOf( E_GPU_DATA_FORMAT_FLAG_DEPTH_STENCIL_BIT );
		}

		const TextureLayout & queryTextureLayout( TextureHandle pTexture ) noexcept
		{
			return pTexture ? pTexture->mTextureLayout : sInvalidTextureLayout;
		}

		ETextureTarget getTextureTargetFromResourceFlags( const Bitmask<resource_flags_value_t> & pTextureResourceFlags )
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

	}

} // namespace ts3::gpuapi
