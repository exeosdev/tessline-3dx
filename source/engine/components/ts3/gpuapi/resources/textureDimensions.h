
#pragma once

#ifndef __TS3_GPUAPI_TEXTURE_DIMENSIONS_H__
#define __TS3_GPUAPI_TEXTURE_DIMENSIONS_H__

#include "commonGPUResourceDefs.h"

namespace ts3::gpuapi
{

	namespace cxdefs
	{

		/// @brief A special value for texture array index indicating all resources in an array.
		/// For a specific mip level, it references a vertical mip slice for a texture array.
		constexpr auto TEX_SUBRESOURCE_ARRAY_INDEX_ALL_TEXTURES = Limits<uint32>::maxValue;

		/// @brief
		constexpr auto TEX_SUBRESOURCE_CUBE_MAP_FACE_ALL_FACES = Limits<uint32>::maxValue;

		/// @brief
		constexpr auto TEX_SUBRESOURCE_DEPTH_ALL_LAYERS = Limits<uint32>::maxValue;

		/// @brief A special value for mip level index indicating all levels a texture contains.
		/// For a given texture, it references all sub-resources in that texture..
		constexpr auto TEX_SUBRESOURCE_MIP_LEVEL_ALL_MIPS = Limits<uint32>::maxValue;

	}

	enum class ETextureClass : enum_default_value_t
	{
		T2D,
		T2DArray,
		T2DMS,
		T2DMSArray,
		T3D,
		TCubeMap,
		Unknown
	};

	enum ETextureCubeMapFace : uint32
	{
		E_TEXTURE_CUBE_MAP_FACE_POSITIVE_X,
		E_TEXTURE_CUBE_MAP_FACE_NEGATIVE_X,
		E_TEXTURE_CUBE_MAP_FACE_POSITIVE_Y,
		E_TEXTURE_CUBE_MAP_FACE_NEGATIVE_Y,
		E_TEXTURE_CUBE_MAP_FACE_POSITIVE_Z,
		E_TEXTURE_CUBE_MAP_FACE_NEGATIVE_Z,
		E_TEXTURE_CUBE_MAP_FACE_FIRST = E_TEXTURE_CUBE_MAP_FACE_POSITIVE_X,
		E_TEXTURE_CUBE_MAP_FACE_LAST = E_TEXTURE_CUBE_MAP_FACE_NEGATIVE_Z,
	};

	struct TextureDimensions
	{
		uint32 width;
		uint32 height;
		uint32 depth;
		uint32 arraySize;
		uint32 mipLevelsNum;
	};

	struct TextureOffset2D
	{
		uint32 mipLevel;
		uint32 x;
		uint32 y;
	};

	struct TextureOffset2DArray : public TextureOffset2D
	{
		uint32 arrayIndex;
	};

	struct TextureOffset3D : public TextureOffset2D
	{
		uint32 z;
	};

	struct TextureOffsetCubeMap : public TextureOffset2D
	{
		uint32 faceIndex;
	};

	struct TextureOffset
	{
		ETextureClass texClass = ETextureClass::Unknown;
		union
		{
			TextureOffset2D uOff2D;
			TextureOffset2DArray uOff2DArray;
			TextureOffset3D uOff3D;
			TextureOffsetCubeMap uOffCubeMap;
		};

		TextureOffset() = default;

		TextureOffset( const TextureOffset2D & pOffset2D, ETextureClass pTexClass = ETextureClass::T2D )
		: texClass( pTexClass )
		, uOff2D( pOffset2D )
		{}

		TextureOffset( const TextureOffset2DArray & pOffset2DArray, ETextureClass pTexClass = ETextureClass::T2D )
		: texClass( pTexClass )
		, uOff2DArray( pOffset2DArray )
		{}

		TextureOffset( const TextureOffset3D & pOffset3D )
		: texClass( ETextureClass::T3D )
		, uOff3D( pOffset3D )
		{}

		TextureOffset( const TextureOffsetCubeMap & pOffsetCubeMap )
		: texClass( ETextureClass::TCubeMap )
		, uOffCubeMap( pOffsetCubeMap )
		{}

		explicit operator bool() const noexcept
		{
			return texClass != ETextureClass::Unknown;
		}
	};

	struct TextureSize2D
	{
		uint32 width;
		uint32 height;

		inline constexpr bool operator==( const TextureSize2D & pRhs ) const noexcept
		{
			return ( width == pRhs.width ) && ( height == pRhs.height );
		}

		inline constexpr bool operator!=( const TextureSize2D & pRhs ) const noexcept
		{
			return ( width != pRhs.width ) || ( height != pRhs.height );
		}
	};

	struct TextureSize2DArray : public TextureSize2D
	{
		uint32 arraySize;

		inline constexpr bool operator==( const TextureSize2DArray & pRhs ) const noexcept
		{
			return ( width == pRhs.width ) && ( height == pRhs.height ) && ( arraySize == pRhs.arraySize );
		}

		inline constexpr bool operator!=( const TextureSize2DArray & pRhs ) const noexcept
		{
			return ( width != pRhs.width ) || ( height != pRhs.height ) || ( arraySize != pRhs.arraySize );
		}
	};

	struct TextureSize3D : public TextureSize2D
	{
		uint32 depth;

		inline constexpr bool operator==( const TextureSize3D & pRhs ) const noexcept
		{
			return ( width == pRhs.width ) && ( height == pRhs.height ) && ( depth == pRhs.depth );
		}

		inline constexpr bool operator!=( const TextureSize3D & pRhs ) const noexcept
		{
			return ( width != pRhs.width ) || ( height != pRhs.height ) || ( depth != pRhs.depth );
		}
	};

	struct TextureSizeCubeMap : public TextureSize2D
	{
		static constexpr uint32 facesNum = 6;
	};

	struct TextureSize
	{
		ETextureClass texClass = ETextureClass::Unknown ;
		union
		{
			TextureSize2D uSz2D;
			TextureSize2DArray uSz2DArray;
			TextureSize3D uSz3D;
			TextureSizeCubeMap uSzCubeMap;
		};

		TextureSize() = default;

		TextureSize( const TextureSize2D & pSize2D, ETextureClass pTexClass = ETextureClass::T2D )
		: texClass( pTexClass )
		, uSz2D( pSize2D )
		{}

		TextureSize( const TextureSize2DArray & pSize2DArray, ETextureClass pTexClass = ETextureClass::T2DArray )
		: texClass( pTexClass )
		, uSz2DArray( pSize2DArray )
		{}

		TextureSize( const TextureSize3D & pSize3D )
		: texClass( ETextureClass::T3D )
		, uSz3D( pSize3D )
		{}

		TextureSize( const TextureSizeCubeMap & pSizeCubeMap )
		: texClass( ETextureClass::TCubeMap )
		, uSzCubeMap( pSizeCubeMap )
		{}

		explicit operator bool() const noexcept
		{
			return texClass != ETextureClass::Unknown;
		}
	};

	struct TextureSubRegion2D
	{
		TextureOffset2D offset;
		TextureSize2D size;
	};

	struct TextureSubRegion2DArray
	{
		TextureOffset2DArray offset;
		TextureSize2DArray size;
	};

	struct TextureSubRegion3D
	{
		TextureOffset3D offset;
		TextureSize3D size;
	};

	struct TextureSubRegionCubeMap
	{
		TextureOffsetCubeMap offset;
		TextureSizeCubeMap size;
	};

	struct TextureSubRegion
	{
		ETextureClass texClass = ETextureClass::Unknown;
		union
		{
			TextureSubRegion2D uSubReg2D;
			TextureSubRegion2DArray uSubReg2DArray;
			TextureSubRegion3D uSubReg3D;
			TextureSubRegionCubeMap uSubRegCubeMap;
		};

		TextureSubRegion() = default;

		TextureSubRegion( const TextureSubRegion2D & pSubReg2D, ETextureClass pTexClass = ETextureClass::T2D )
		: texClass( pTexClass )
		, uSubReg2D( pSubReg2D )
		{}

		TextureSubRegion( const TextureSubRegion2DArray & pSubReg2DArray, ETextureClass pTexClass = ETextureClass::T2D )
		: texClass( pTexClass )
		, uSubReg2DArray( pSubReg2DArray )
		{}

		TextureSubRegion( const TextureSubRegion3D & pSubReg3D )
		: texClass( ETextureClass::T3D )
		, uSubReg3D( pSubReg3D )
		{}

		TextureSubRegion( const TextureSubRegionCubeMap & pSubRegCubeMap )
		: texClass( ETextureClass::TCubeMap )
		, uSubRegCubeMap( pSubRegCubeMap )
		{}

		explicit operator bool() const noexcept
		{
			return texClass != ETextureClass::Unknown;
		}
	};

	struct TextureSubResource2D
	{
		/// A mip level index.
		uint32 mipLevel;
	};

	struct TextureSubResource2DArray : public TextureSubResource2D
	{
		/// An index within a texture array. Use cxdefs::TEX_SUBRESOURCE_INDEX_MIP_VERTICAL_SLICE for a vertical mip slice.
		uint32 arrayIndex;
	};

	struct TextureSubResource3D : public TextureSubResource2D
	{
		///
		uint32 depthLayerIndex;
	};

	struct TextureSubResourceCubeMap : public TextureSubResource2D
	{
		/// An index of the cube map face. Use cxdefs::TEX_SUBRESOURCE_INDEX_MIP_VERTICAL_SLICE for a vertical mip slice.
		uint32 faceIndex;
	};

	struct TextureSubResource
	{
		ETextureClass texClass = ETextureClass::Unknown;
		union
		{
			TextureSubResource2D uSubRes2D;
			TextureSubResource2DArray uSubRes2DArray;
			TextureSubResource3D uSubRes3D;
			TextureSubResourceCubeMap uSubResCubeMap;
		};

		TextureSubResource() = default;

		TextureSubResource( const TextureSubResource2D & pSubRes2D, ETextureClass pTexClass = ETextureClass::T2D )
		: texClass( pTexClass )
		, uSubRes2D( pSubRes2D )
		{}

		TextureSubResource( const TextureSubResource2DArray & pSubRes2DArray, ETextureClass pTexClass = ETextureClass::T2DArray )
		: texClass( pTexClass )
		, uSubRes2DArray( pSubRes2DArray )
		{}

		TextureSubResource( const TextureSubResource3D & pSubRes3D )
		: texClass( ETextureClass::T3D )
		, uSubRes3D( pSubRes3D )
		{}

		TextureSubResource( const TextureSubResourceCubeMap & pSubResCubeMap )
		: texClass( ETextureClass::TCubeMap )
		, uSubResCubeMap( pSubResCubeMap )
		{}

		explicit operator bool() const noexcept
		{
			return texClass != ETextureClass::Unknown;
		}

		void reset()
		{
			texClass = ETextureClass::Unknown;
		}
	};

	namespace rcutil
	{

		TS3_GPUAPI_API_NO_DISCARD TextureDimensions queryMipLevelDimensions(
				const TextureDimensions & pDimensions,
				uint32 pMipLevel ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD TextureSubResource getTextureSubResourceWholeTexture( TextureHandle pTexture ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubRegion(
				TextureHandle pTexture,
				const TextureSubRegion & pSubRegion ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubRegion(
				const TextureDimensions & pDimensions,
				const TextureSubRegion & pSubRegion ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubRegion2D(
				const TextureDimensions & pDimensions,
				const TextureSubRegion2D & pSubRegion ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubRegion2DArray(
				const TextureDimensions & pDimensions,
				const TextureSubRegion2DArray & pSubRegion ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubRegion2DMS(
				const TextureDimensions & pDimensions,
				const TextureSubRegion2D & pSubRegion ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubRegion3D(
				const TextureDimensions & pDimensions,
				const TextureSubRegion3D & pSubRegion ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubRegionCubeMap(
				const TextureDimensions & pDimensions,
				const TextureSubRegionCubeMap & pSubRegion ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubResource(
				TextureHandle pTexture,
				const TextureSubResource & pSubResource ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubResource(
				const TextureDimensions & pDimensions,
				const TextureSubResource & pSubResource ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubResource2D(
				const TextureDimensions & pDimensions,
				const TextureSubResource2D & pSubResource ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubResource2DArray(
				const TextureDimensions & pDimensions,
				const TextureSubResource2DArray & pSubResource ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubResource2DMS(
				const TextureDimensions & pDimensions,
				const TextureSubResource2D & pSubResource ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubResource3D(
				const TextureDimensions & pDimensions,
				const TextureSubResource3D & pSubResource ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkTextureSubResourceCubeMap(
				const TextureDimensions & pDimensions,
				const TextureSubResourceCubeMap & pSubResource ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubRegion(
				const TextureSubRegion & pSubRegion1,
				const TextureSubRegion & pSubRegion2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubRegion2D(
				const TextureSubRegion2D & pSubRegion1,
				const TextureSubRegion2D & pSubRegion2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubRegion2DArray(
				const TextureSubRegion2DArray & pSubRegion1,
				const TextureSubRegion2DArray & pSubRegion2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubRegion2DMS(
				const TextureSubRegion2D & pSubRegion1,
				const TextureSubRegion2D & pSubRegion2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubRegion3D(
				const TextureSubRegion3D & pSubRegion1,
				const TextureSubRegion3D & pSubRegion2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubRegionCubeMap(
				const TextureSubRegionCubeMap & pSubRegion1,
				const TextureSubRegionCubeMap & pSubRegion2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubResource(
				const TextureSubResource & pSubResource1,
				const TextureSubResource & pSubResource2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubResource2D(
				const TextureSubResource2D & pSubResource1,
				const TextureSubResource2D & pSubResource2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubResource2DArray(
				const TextureSubResource2DArray & pSubResource1,
				const TextureSubResource2DArray & pSubResource2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubResource2DMS(
				const TextureSubResource2D & pSubResource1,
				const TextureSubResource2D & pSubResource2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubResource3D(
				const TextureSubResource3D & pSubResource1,
				const TextureSubResource3D & pSubResource2 ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool cmpEqTextureSubResourceCubeMap(
				const TextureSubResourceCubeMap & pSubResource1,
				const TextureSubResourceCubeMap & pSubResource2 ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_TEXTURE_DIMENSIONS_H__
