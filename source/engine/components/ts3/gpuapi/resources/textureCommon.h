
#pragma once

#ifndef __TS3_GPUAPI_TEXTURE_COMMON_H__
#define __TS3_GPUAPI_TEXTURE_COMMON_H__

#include "commonGPUResourceDefs.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( Texture );

	constexpr uint32 cxTextureMipLevelAll = Limits<uint32>::maxValue;

	enum class ETextureDimensionClass : enum_default_value_t
	{
		Texture2D,
		Texture2DArray,
		Texture2DMS,
		Texture2DMSArray,
		Texture3D,
		TextureCubeMap,
		TextureUnknown
	};

	enum class ETextureCubeMapFace : size_t
	{
		PositiveX,
		NegativeX,
		PositiveY,
		NegativeY,
		PositiveZ,
		NegativeZ
	};

	enum ETextureDataCopyFlags : uint32
	{
		E_TEXTURE_DATA_COPY_FLAG_MODE_INVALIDATE_BIT = E_GPU_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT,
		E_TEXTURE_DATA_COPY_FLAGS_DEFAULT = 0,
	};

	/// @brief Bind flags for texture resources. Used to specify how textures are bound to the pipeline.
	///
	/// Note:
	/// Just like in case of EGPUBufferBindFlags, these flags contain required E_GPU_RESOURCE_USAGE_FLAG bits as well.
	enum ETextureBindFlags : resource_flags_value_t
	{
		// Bind flag for using a texture as a sampled image available in one or more shader stages.
		// Such texture is accessible via one of the valid TX input registers and can be read through a sampler object.
		// Implies SHADER_INPUT usage bit.
		E_TEXTURE_BIND_FLAG_SHADER_INPUT_SAMPLED_IMAGE_BIT             = 0x010000 | E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT,

		// Bind flag for using a texture as a color attachment image, modified as part of the FBO rendering.
		// Implies RENDER_TARGET_COLOR usage bit.
		E_TEXTURE_BIND_FLAG_RENDER_TARGET_COLOR_ATTACHMENT_BIT         = 0x020000 | E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT,

		// Bind flag for using a texture as a depth/stencil attachment image, modified as part of the FBO rendering
		// (target storage for depth/stencil values) or read/modified during the depth and/or stencil tests.
		// Implies RENDER_TARGET_DEPTH_STENCIL usage bit.
		E_TEXTURE_BIND_FLAG_RENDER_TARGET_DEPTH_STENCIL_ATTACHMENT_BIT = 0x040000 | E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_STENCIL_BIT,

		// Bind flag for using a texture as a source in transfer operations. @See E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT.
		E_TEXTURE_BIND_FLAG_TRANSFER_SOURCE_IMAGE_BIT  = E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT,

		// Bind flag for using a texture as a target in transfer operations. @See E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_TARGET_BIT.
		E_TEXTURE_BIND_FLAG_TRANSFER_TARGET_IMAGE_BIT  = E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_TARGET_BIT,

		// Mask with all valid BIND_FLAG bits set.
		E_TEXTURE_BIND_MASK_ALL = 0x070000 | E_GPU_RESOURCE_USAGE_MASK_ALL,
	};

	/// @brief Represents all valid targets for texture resources. Corresponding E_TEXTURE_BIND_FLAGs are used as values.
	enum class ETextureTarget : resource_flags_value_t
	{
		ShaderInputSampledImage            = E_TEXTURE_BIND_FLAG_SHADER_INPUT_SAMPLED_IMAGE_BIT,
		RenderTargetColorAttachment        = E_TEXTURE_BIND_FLAG_RENDER_TARGET_COLOR_ATTACHMENT_BIT,
		RenderTargetDepthStencilAttachment = E_TEXTURE_BIND_FLAG_RENDER_TARGET_DEPTH_STENCIL_ATTACHMENT_BIT,
		TransferSourceImage                = E_TEXTURE_BIND_FLAG_TRANSFER_SOURCE_IMAGE_BIT,
		TransferTargetImage                = E_TEXTURE_BIND_FLAG_TRANSFER_TARGET_IMAGE_BIT,
		Unknown = 0
	};

	struct TextureDimensions
	{
		uint32 width;
		uint32 height;
		uint32 depth;
		uint32 arraySize;
		uint32 mipLevelsNum;
	};

	struct TextureLayout
	{
		ETextureDimensionClass dimensionClass;
		TextureDimensions dimensions;
		uint32 msaaLevel;
		uint32 bitsPerPixel;
		uint32 storageSize;
		ETextureFormat pixelFormat;
	};

	struct TextureOffset2D
	{
		uint32 mipLevel;
		uint32 x;
		uint32 y;
	};

	struct TextureOffset2DArray
	{
		uint32 mipLevel;
		uint32 x;
		uint32 y;
		uint32 arrayIndex;
	};

	struct TextureOffset3D
	{
		uint32 mipLevel;
		uint32 x;
		uint32 y;
		uint32 z;
	};

	struct TextureOffsetCubeMap
	{
		uint32 mipLevel;
		uint32 x;
		uint32 y;
		union
		{
			uint32 uFaceIndex;
			ETextureCubeMapFace uFaceID;
		};
	};

	union TextureOffset
	{
		TextureOffset2D      u2D;
		TextureOffset2DArray u2DArray;
		TextureOffset3D      u3D;
		TextureOffsetCubeMap uCubeMap;
	};

	struct TextureSize2D
	{
		uint32 width;
		uint32 height;
	};

	struct TextureSize2DArray
	{
		uint32 width;
		uint32 height;
		uint32 sliceCount;
	};

	struct TextureSize3D
	{
		uint32 width;
		uint32 height;
		uint32 depth;
	};

	struct TextureSizeCubeMap
	{
		uint32 width;
		uint32 height;
	};

	union TextureSize
	{
		TextureSize2D      u2D;
		TextureSize2DArray u2DArray;
		TextureSize3D      u3D;
		TextureSizeCubeMap uCubeMap;
	};

	struct TextureSubRegion
	{
		TextureOffset offset;
		TextureSize size;
	};
	
	struct TextureSubResource2D
	{
		uint32 mipLevel;
	};

	struct TextureSubResource2DArray
	{
		uint32 mipLevel;
		uint32 arrayIndex;
	};

	struct TextureSubResource3D
	{
		uint32 mipLevel;
		uint32 depthLayerIndex;
	};

	struct TextureSubResourceCubeMap
	{
		uint32 mipLevel;
		union
		{
			uint32 uFaceIndex;
			ETextureCubeMapFace uFaceID;
		};
	};

	struct TextureSubResource
	{
		TextureSubResource2D      u2D;
		TextureSubResource2DArray u2DArray;
		TextureSubResource3D      u3D;
		TextureSubResourceCubeMap uCubeMap;
	};

	struct TextureMipSubLevelInitDataDesc : public ResourceInputDataDesc
	{
		uint32 mipWidth;
		uint32 mipHeight;
		uint32 mipDepth;
		uint32 mipLevelIndex;
	};

	struct TextureSubTextureInitDataDesc
	{
		using MipLevelInitDataDescArray = std::array<TextureMipSubLevelInitDataDesc, E_GPU_SYSTEM_METRIC_TEXTURE_MAX_MIP_LEVELS_NUM>;
		MipLevelInitDataDescArray mipLevelInitDataArray;
		uint32 subTextureIndex;
	};

	struct TextureInitDataDesc
	{
	public:
		TextureInitDataDesc() = default;

		TextureInitDataDesc( TextureInitDataDesc && pSource );
		TextureInitDataDesc & operator=( TextureInitDataDesc && pRhs );

		TextureInitDataDesc( const TextureInitDataDesc & pSource );
		TextureInitDataDesc & operator=( const TextureInitDataDesc & pRhs );

		using SubTextureInitDataDescArray = std::vector<TextureSubTextureInitDataDesc>;

		// Pointer to one or more TextureSubTextureInitDataDesc objects with init data for sub-textures.
		// Points to either _subTextureInitDataSingleTexture member or the beginning of the data in the
		// _subTextureInitDataTextureArray vector. This is an optimisation done for single textures
		// (arraySize=1) in order to prevent dynamic allocation of memory. In few cases where the engine
		// is used for allocating huge number of small non-array textures, this turned out to be an issue.
		TextureSubTextureInitDataDesc * subTextureInitDataBasePtr = nullptr;

		explicit operator bool() const
		{
			return subTextureInitDataBasePtr != nullptr;
		}

		void initialize( const TextureDimensions & pDimensions );

		void swap( TextureInitDataDesc & pOther );

		bool isArray() const;

		bool empty() const;

		TextureInitDataDesc copy() const;

	private:
		// Init data for single-layer (non-array) texture.
		TextureSubTextureInitDataDesc _subTextureInitData;
		// Vector of init data structs for array textures.
		SubTextureInitDataDescArray _subTextureInitDataArray;
	};

	struct TextureInputDataDesc : public ResourceInputDataDesc
	{
		EPixelDataLayout pixelDataLayout;
		EBaseDataType pixelDataType;
	};

	struct TextureCreateInfo : public ResourceCreateInfo
	{
		ETextureDimensionClass dimensionClass;
		uint32 msaaLevel = 0;
		TextureDimensions dimensions{ 0, 0, 1, 1, 1 };
		ETextureFormat pixelFormat;
		ETextureTarget initialTarget;
		TextureInitDataDesc initDataDesc;

		explicit operator bool() const
		{
			return ( dimensionClass != ETextureDimensionClass::TextureUnknown ) && ( pixelFormat != ETextureFormat::UNKNOWN );
		}
	};

	struct TextureDataCopyDesc
	{
		ETextureDimensionClass dimensionClass;
		Bitmask<ETextureDataCopyFlags> flags = E_TEXTURE_DATA_COPY_FLAGS_DEFAULT;
	};

	struct TextureSubDataCopyDesc
	{
		ETextureDimensionClass dimensionClass;
		TextureSubRegion sourceTextureSubRegion;
		TextureOffset targetTextureOffset;
		Bitmask<ETextureDataCopyFlags> flags = E_TEXTURE_DATA_COPY_FLAGS_DEFAULT;
	};

	struct TextureDataUploadDesc
	{
		ETextureDimensionClass dimensionClass;
		TextureInputDataDesc inputDataDesc;
		Bitmask<ETextureDataCopyFlags> flags = E_TEXTURE_DATA_COPY_FLAGS_DEFAULT;
	};

	struct TextureSubDataUploadDesc
	{
		ETextureDimensionClass dimensionClass;
		TextureSubRegion textureSubRegion;
		TextureInputDataDesc inputDataDesc;
		Bitmask<ETextureDataCopyFlags> flags = E_TEXTURE_DATA_COPY_FLAGS_DEFAULT;
	};

	namespace TextureUtils
	{

		TS3_GPUAPI_API TextureDimensions queryMipLevelDimensions( const TextureDimensions & pDimensions, uint32 pMipLevel );

		TS3_GPUAPI_API bool validateTextureSubRegion( ETextureDimensionClass pDimensionClass,
		                                              const TextureDimensions & pDimensions,
		                                              const TextureSubRegion & pSubRegion );

		TS3_GPUAPI_API bool validateTextureSubRegion2D( const TextureDimensions & pDimensions,
		                                                const TextureSubRegion & pSubRegion );

		TS3_GPUAPI_API bool validateTextureSubRegion2DArray( const TextureDimensions & pDimensions,
		                                                     const TextureSubRegion & pSubRegion );

		TS3_GPUAPI_API bool validateTextureSubRegion2DMS( const TextureDimensions & pDimensions,
		                                                  const TextureSubRegion & pSubRegion );

		TS3_GPUAPI_API bool validateTextureSubRegion3D( const TextureDimensions & pDimensions,
		                                                const TextureSubRegion & pSubRegion );

		TS3_GPUAPI_API bool validateTextureSubRegionCubeMap( const TextureDimensions & pDimensions,
		                                                     const TextureSubRegion & pSubRegion );

		TS3_GPUAPI_API bool validateTextureSubResource( ETextureDimensionClass pDimensionClass,
		                                                const TextureDimensions & pDimensions,
		                                                const TextureSubResource & pSubResource );

		TS3_GPUAPI_API bool validateTextureSubResource2D( const TextureDimensions & pDimensions,
		                                                  const TextureSubResource & pSubResource );

		TS3_GPUAPI_API bool validateTextureSubResource2DArray( const TextureDimensions & pDimensions,
		                                                       const TextureSubResource & pSubResource );

		TS3_GPUAPI_API bool validateTextureSubResource2DMS( const TextureDimensions & pDimensions,
		                                                    const TextureSubResource & pSubResource );

		TS3_GPUAPI_API bool validateTextureSubResource3D( const TextureDimensions & pDimensions,
		                                                  const TextureSubResource & pSubResource );

		TS3_GPUAPI_API bool validateTextureSubResourceCubeMap( const TextureDimensions & pDimensions,
		                                                       const TextureSubResource & pSubResource );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_TEXTURE_COMMON_H__
