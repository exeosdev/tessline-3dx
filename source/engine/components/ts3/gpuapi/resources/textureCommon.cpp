
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
			ETextureFormat::UNKNOWN,
			TextureDimensions {
				0, 0, 0, 0, 0
			},
			0,
			0,
			0
		};

		bool checkTextureFormatAndBindFlagsCompatibility(
				ETextureFormat pTextureFormat,
				Bitmask<resource_flags_value_t> pBindFlags )
		{
			if( pBindFlags.isSet( E_GPU_RESOURCE_USAGE_MASK_RENDER_TARGET_DEPTH_STENCIL ) )
			{
				return ( pTextureFormat == ETextureFormat::D24UNS8U );
			}
			else if( pBindFlags.isSet( E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_BIT ) )
			{
				return
					( pTextureFormat == ETextureFormat::D16UN ) ||
					( pTextureFormat == ETextureFormat::D24UNX8 ) ||
					( pTextureFormat == ETextureFormat::D32F );
			}
			else if( pBindFlags.isSet( E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_STENCIL_BIT ) )
			{
				return ( pTextureFormat == ETextureFormat::X24S8U );
			}

			return false;
		}

		const TextureLayout & queryTextureLayout( TextureHandle pTexture ) noexcept
		{
			return pTexture ? pTexture->mTextureLayout : sInvalidTextureLayout;
		}

		RenderTargetTextureLayout queryRenderTargetTextureLayoutForTexture( TextureHandle pTexture ) noexcept
		{
			return pTexture ? queryRenderTargetTextureLayoutForTexture( pTexture->mTextureLayout ) : RenderTargetTextureLayout{};
		}

		RenderTargetTextureLayout queryRenderTargetTextureLayoutForTexture( const TextureLayout & pTextureLayout ) noexcept
		{
			RenderTargetTextureLayout rttLayout{};
			rttLayout.internalFormat = pTextureLayout.internalFormat;
			rttLayout.msaaLevel = pTextureLayout.msaaLevel;
			rttLayout.imageRect.width = pTextureLayout.dimensions.width;
			rttLayout.imageRect.height = pTextureLayout.dimensions.height;
			return rttLayout;
		}

	}

} // namespace ts3::gpuapi
