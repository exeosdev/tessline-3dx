
#pragma once

#ifndef __TS3_GPUAPI_TEXTURE_COMMON_H__
#define __TS3_GPUAPI_TEXTURE_COMMON_H__

#include "textureDimensions.h"

namespace ts3::gpuapi
{

	struct TextureCreateInfo;
	struct RenderTargetTextureCreateInfo;

	ts3DeclareClassHandle( Texture );
	ts3DeclareClassHandle( RenderTargetTexture );

	namespace cxdefs
	{

		/// @brief Identifies all mip levels available in a texture resource. Can be used for all resource accesses.
		constexpr uint32 TEXTURE_MIP_LEVEL_ALL = Limits<uint32>::maxValue;

		inline constexpr TextureSize2D TEXTURE_SIZE_2D_UNDEFINED{ Limits<uint32>::maxValue, Limits<uint32>::maxValue };

		inline constexpr uint32 TEXTURE_MSAA_LEVEL_UNDEFINED = Limits<uint32>::maxValue;

	}

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
		//
		E_TEXTURE_BIND_FLAG_COMMON_TEXTURE_BIT = 0x010000,

		// Bind flag for using a texture as a sampled image available in one or more shader stages.
		// Such texture is accessible via one of the valid TX input registers and can be read through a sampler object.
		// Implies SHADER_INPUT usage bit.
		E_TEXTURE_BIND_FLAG_SHADER_INPUT_SAMPLED_IMAGE_BIT =
				E_TEXTURE_BIND_FLAG_COMMON_TEXTURE_BIT |
				E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT,

		// Bind flag for using a texture as a color attachment image, modified as part of the FBO rendering.
		// Implies RENDER_TARGET_COLOR usage bit.
		E_TEXTURE_BIND_FLAG_RENDER_TARGET_COLOR_ATTACHMENT_BIT =
				E_TEXTURE_BIND_FLAG_COMMON_TEXTURE_BIT |
				E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT,

		//
		E_TEXTURE_BIND_FLAG_RENDER_TARGET_DEPTH_ATTACHMENT_BIT =
				E_TEXTURE_BIND_FLAG_COMMON_TEXTURE_BIT |
				E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_BIT,

		// Bind flag for using a texture as a depth/stencil attachment image, modified as part of the FBO rendering
		// (target storage for depth/stencil values) or read/modified during the depth and/or stencil tests.
		// Implies RENDER_TARGET_DEPTH_STENCIL usage bit.
		E_TEXTURE_BIND_FLAG_RENDER_TARGET_DEPTH_STENCIL_ATTACHMENT_BIT =
				E_TEXTURE_BIND_FLAG_COMMON_TEXTURE_BIT |
				E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_STENCIL_BIT,

		// Bind flag for using a texture as a source in transfer operations. @See E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT.
		E_TEXTURE_BIND_FLAG_TRANSFER_SOURCE_IMAGE_BIT =
				E_TEXTURE_BIND_FLAG_COMMON_TEXTURE_BIT |
				E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT,

		// Bind flag for using a texture as a target in transfer operations. @See E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_TARGET_BIT.
		E_TEXTURE_BIND_FLAG_TRANSFER_TARGET_IMAGE_BIT =
				E_TEXTURE_BIND_FLAG_COMMON_TEXTURE_BIT |
				E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_TARGET_BIT,
	};
	
	enum ETextureInitFlags : uint32
	{
		E_TEXTURE_INIT_FLAG_GENERATE_MIPMAPS_BIT = 0x01
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

	struct RenderTargetTextureLayout
	{
		ETextureFormat internalDataFormat = ETextureFormat::UNKNOWN;
		uint32 msaaLevel;
		TextureSize2D bufferSize;

		explicit operator bool() const noexcept
		{
			return internalDataFormat != ETextureFormat::UNKNOWN;
		}
	};

	struct TextureLayout
	{
		ETextureClass texClass = ETextureClass::Unknown;
		TextureDimensions dimensions;
		uint32 msaaLevel;
		uint32 bitsPerPixel;
		uint32 storageSize;
		ETextureFormat pixelFormat;

		explicit operator bool() const noexcept
		{
			return texClass != ETextureClass::Unknown;
		}
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
		using MipLevelInitDataDescArray = std::array<TextureMipSubLevelInitDataDesc, gpm::TEXTURE_MAX_MIP_LEVELS_NUM>;
		MipLevelInitDataDescArray mipLevelInitDataArray;
		uint32 subTextureIndex;
	};

	struct TextureInitDataDesc
	{
	public:
		TextureInitDataDesc() = default;

		TextureInitDataDesc( TextureInitDataDesc && pSrcObject );
		TextureInitDataDesc & operator=( TextureInitDataDesc && pRhs );

		TextureInitDataDesc( const TextureInitDataDesc & pSrcObject );
		TextureInitDataDesc & operator=( const TextureInitDataDesc & pRhs );

		using SubTextureInitDataDescArray = std::vector<TextureSubTextureInitDataDesc>;

		// Pointer to one or more TextureSubTextureInitDataDesc objects with init data for sub-textures.
		// Points to either _subTextureInitDataSingleTexture member or the beginning of the data in the
		// _subTextureInitDataTextureArray vector. This is an optimisation done for single textures
		// (arraySize=1) in order to prevent dynamic allocation of memory. In few cases where the engine
		// is used for allocating huge number of small non-array textures, this turned out to be an issue.
		TextureSubTextureInitDataDesc * subTextureInitDataBasePtr = nullptr;

		Bitmask<ETextureInitFlags> textureInitFlags = E_TEXTURE_INIT_FLAG_GENERATE_MIPMAPS_BIT;

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
		ETextureClass texClass;
		uint32 msaaLevel = 0;
		TextureDimensions dimensions{ 0, 0, 1, 1, 1 };
		ETextureFormat pixelFormat;
		ETextureTarget initialTarget;
		TextureInitDataDesc initDataDesc;

		explicit operator bool() const
		{
			return ( texClass != ETextureClass::Unknown ) && ( pixelFormat != ETextureFormat::UNKNOWN );
		}
	};

	struct TextureDataCopyDesc
	{
		ETextureClass texClass;
		Bitmask<ETextureDataCopyFlags> flags = E_TEXTURE_DATA_COPY_FLAGS_DEFAULT;
	};

	struct TextureSubDataCopyDesc
	{
		ETextureClass texClass;
		TextureSubRegion sourceTextureSubRegion;
		TextureOffset targetTextureOffset;
		Bitmask<ETextureDataCopyFlags> flags = E_TEXTURE_DATA_COPY_FLAGS_DEFAULT;
	};

	struct TextureDataUploadDesc
	{
		ETextureClass texClass;
		TextureInputDataDesc inputDataDesc;
		Bitmask<ETextureDataCopyFlags> flags = E_TEXTURE_DATA_COPY_FLAGS_DEFAULT;
	};

	struct TextureSubDataUploadDesc
	{
		ETextureClass texClass;
		TextureSubRegion textureSubRegion;
		TextureInputDataDesc inputDataDesc;
		Bitmask<ETextureDataCopyFlags> flags = E_TEXTURE_DATA_COPY_FLAGS_DEFAULT;
	};

	namespace rcutil
	{

		TS3_GPUAPI_API_NO_DISCARD bool checkRenderTargetTextureColorFormat( ETextureFormat pFormat ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD bool checkRenderTargetTextureDepthStencilFormat( ETextureFormat pFormat ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD const TextureLayout & queryTextureLayout( TextureHandle pTexture ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD RenderTargetTextureLayout queryRenderTargetTextureLayoutForTexture( TextureHandle pTexture ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD RenderTargetTextureLayout queryRenderTargetTextureLayoutForTexture( const TextureLayout & pTextureLayout ) noexcept;

		TS3_GPUAPI_API_NO_DISCARD ETextureTarget getTextureTargetFromResourceFlags( const Bitmask<resource_flags_value_t> & pTextureResourceFlags );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_TEXTURE_COMMON_H__
