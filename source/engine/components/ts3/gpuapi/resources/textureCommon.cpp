
#include "textureCommon.h"
#include <ts3/stdext/utilities.h>

namespace ts3::gpuapi
{

	TextureInitDataDesc::TextureInitDataDesc( TextureInitDataDesc && pSource )
	{
		if( !pSource.empty() )
		{
			if( pSource.isArray() )
			{
				_subTextureInitDataArray = std::move( pSource._subTextureInitDataArray );
				subTextureInitDataBasePtr = _subTextureInitDataArray.data();
			}
			else
			{
				_subTextureInitData = pSource._subTextureInitData;
				subTextureInitDataBasePtr = &_subTextureInitData;
			}
			pSource.subTextureInitDataBasePtr = nullptr;
		}
	}

	TextureInitDataDesc & TextureInitDataDesc::operator=( TextureInitDataDesc && pRhs )
	{
		TextureInitDataDesc( std::move( pRhs ) ).swap( *this );
		return *this;
	}

	TextureInitDataDesc::TextureInitDataDesc( const TextureInitDataDesc & pSource )
	{
		if( !pSource.empty() )
		{
			if( pSource.isArray() )
			{
				_subTextureInitDataArray = pSource._subTextureInitDataArray;
				subTextureInitDataBasePtr = _subTextureInitDataArray.data();
			}
			else
			{
				_subTextureInitData = pSource._subTextureInitData;
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


	TextureDimensions TextureUtils::queryMipLevelDimensions( const TextureDimensions & pDimensions, uint32 pMipLevel )
	{
		TextureDimensions result;
		result.arraySize = pDimensions.arraySize;
		result.mipLevelsNum = pDimensions.mipLevelsNum;

		if( pMipLevel >= pDimensions.mipLevelsNum )
		{
			result.width = 0;
			result.height = 0;
			result.depth = 0;
		}
		else
		{
			result.width = pDimensions.width;
			result.height = pDimensions.height;
			result.depth = pDimensions.depth;

			for( uint32 mipLevelIndex = 0; mipLevelIndex < pMipLevel; ++mipLevelIndex )
			{
				result.width = getMaxOf( 1u, result.width  / 2 );
				result.height= getMaxOf( 1u, result.height / 2 );
				result.depth = getMaxOf( 1u, result.depth  / 2 );
			}
		}

		return result;
	}

	bool TextureUtils::validateTextureSubRegion( ETextureDimensionClass pDimensionClass,
	                                             const TextureDimensions & pDimensions,
	                                             const TextureSubRegion & pSubRegion )
	{
		if( pDimensionClass == ETextureDimensionClass::Texture2D )
		{
			if( !validateTextureSubRegion2D( pDimensions, pSubRegion ) )
			{
				return false;
			}
		}
		else if( pDimensionClass == ETextureDimensionClass::Texture2DArray )
		{
			if( !validateTextureSubRegion2DArray( pDimensions, pSubRegion ) )
			{
				return false;
			}
		}
		else if( pDimensionClass == ETextureDimensionClass::Texture2DMS )
		{
			if( !validateTextureSubRegion2DMS( pDimensions, pSubRegion ) )
			{
				return false;
			}
		}
		else if( pDimensionClass == ETextureDimensionClass::Texture3D )
		{
			if( !validateTextureSubRegion3D( pDimensions, pSubRegion ) )
			{
				return false;
			}
		}
		else if( pDimensionClass == ETextureDimensionClass::TextureCubeMap )
		{
			if( !validateTextureSubRegionCubeMap( pDimensions, pSubRegion ) )
			{
				return false;
			}
		}
		else
		{
			ts3DebugInterrupt();
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubRegion2D( const TextureDimensions & pDimensions,
	                                               const TextureSubRegion & pSubRegion )
	{
		if( pSubRegion.offset.u2D.mipLevel >= pDimensions.mipLevelsNum )
		{
			return false;
		}

		auto mipLevelDimensions = queryMipLevelDimensions( pDimensions, pSubRegion.offset.u2D.mipLevel );

		if( pSubRegion.offset.u2D.x > mipLevelDimensions.width )
		{
			return false;
		}

		if( pSubRegion.offset.u2D.y > mipLevelDimensions.height )
		{
			return false;
		}

		if( pSubRegion.size.u2D.width > ( mipLevelDimensions.width - pSubRegion.offset.u2D.x ) )
		{
			return false;
		}

		if( pSubRegion.size.u2D.height > ( mipLevelDimensions.height - pSubRegion.offset.u2D.y ) )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubRegion2DArray( const TextureDimensions & pDimensions,
	                                                    const TextureSubRegion & pSubRegion )
	{
		if( pSubRegion.offset.u2DArray.arrayIndex >= pDimensions.arraySize )
		{
			return false;
		}

		if( !validateTextureSubRegion2D( pDimensions, pSubRegion ) )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubRegion2DMS( const TextureDimensions & pDimensions,
	                                                 const TextureSubRegion & pSubRegion )
	{
		if( pSubRegion.offset.u2D.mipLevel != 0 )
		{
			return false;
		}

		if( !validateTextureSubRegion2D( pDimensions, pSubRegion ) )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubRegion3D( const TextureDimensions & pDimensions,
	                                               const TextureSubRegion & pSubRegion )
	{
		if( pSubRegion.offset.u3D.mipLevel >= pDimensions.mipLevelsNum )
		{
			return false;
		}

		auto mipLevelDimensions = queryMipLevelDimensions( pDimensions, pSubRegion.offset.u3D.mipLevel );

		if( pSubRegion.offset.u3D.x > mipLevelDimensions.width )
		{
			return false;
		}

		if( pSubRegion.offset.u3D.y > mipLevelDimensions.height )
		{
			return false;
		}

		if( pSubRegion.offset.u3D.z > mipLevelDimensions.depth )
		{
			return false;
		}

		if( pSubRegion.size.u3D.width > ( mipLevelDimensions.width - pSubRegion.offset.u3D.x ) )
		{
			return false;
		}

		if( pSubRegion.size.u3D.height > ( mipLevelDimensions.height - pSubRegion.offset.u3D.y ) )
		{
			return false;
		}

		if( pSubRegion.size.u3D.depth > ( mipLevelDimensions.depth - pSubRegion.offset.u3D.z ) )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubRegionCubeMap( const TextureDimensions & pDimensions,
	                                                    const TextureSubRegion & pSubRegion )
	{
		if( pSubRegion.offset.uCubeMap.uFaceIndex > 6 )
		{
			return false;
		}

		if( !validateTextureSubRegion2D( pDimensions, pSubRegion ) )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubResource( ETextureDimensionClass pDimensionClass,
	                                               const TextureDimensions & pDimensions,
	                                               const TextureSubResource & pSubResource )
	{
		if( pDimensionClass == ETextureDimensionClass::Texture2D )
		{
			if( !validateTextureSubResource2D( pDimensions, pSubResource ) )
			{
				return false;
			}
		}
		else if( pDimensionClass == ETextureDimensionClass::Texture2DArray )
		{
			if( !validateTextureSubResource2DArray( pDimensions, pSubResource ) )
			{
				return false;
			}
		}
		else if( pDimensionClass == ETextureDimensionClass::Texture2DMS )
		{
			if( !validateTextureSubResource2DMS( pDimensions, pSubResource ) )
			{
				return false;
			}
		}
		else if( pDimensionClass == ETextureDimensionClass::Texture3D )
		{
			if( !validateTextureSubResource3D( pDimensions, pSubResource ) )
			{
				return false;
			}
		}
		else if( pDimensionClass == ETextureDimensionClass::TextureCubeMap )
		{
			if( !validateTextureSubResourceCubeMap( pDimensions, pSubResource ) )
			{
				return false;
			}
		}
		else
		{
			ts3DebugInterrupt();
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubResource2D( const TextureDimensions & pDimensions,
	                                                 const TextureSubResource & pSubResource )
	{
		if( pSubResource.u2D.mipLevel >= pDimensions.mipLevelsNum )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubResource2DArray( const TextureDimensions & pDimensions,
	                                                      const TextureSubResource & pSubResource )
	{
		if( pSubResource.u2DArray.arrayIndex >= pDimensions.arraySize )
		{
			return false;
		}

		if( pSubResource.u2DArray.mipLevel >= pDimensions.mipLevelsNum )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubResource2DMS( const TextureDimensions & pDimensions,
	                                                   const TextureSubResource & pSubResource )
	{
		if( pSubResource.u2D.mipLevel != 0 )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubResource3D( const TextureDimensions & pDimensions,
	                                                 const TextureSubResource & pSubResource )
	{
		if( pSubResource.u3D.depthLayerIndex >= pDimensions.depth )
		{
			return false;
		}

		if( pSubResource.u3D.mipLevel >= pDimensions.mipLevelsNum )
		{
			return false;
		}

		return true;
	}

	bool TextureUtils::validateTextureSubResourceCubeMap( const TextureDimensions & pDimensions,
	                                                      const TextureSubResource & pSubResource )
	{
		if( pSubResource.uCubeMap.uFaceIndex >= 6 )
		{
			return false;
		}

		if( pSubResource.uCubeMap.mipLevel >= pDimensions.mipLevelsNum )
		{
			return false;
		}

		return true;
	}

} // namespace ts3::gpuapi
