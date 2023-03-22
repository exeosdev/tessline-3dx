
#include "renderTargetTexture.h"
#include "texture.h"

namespace ts3::gpuapi
{

	RenderTargetTexture::RenderTargetTexture(
			GPUDevice & pGPUDevice,
			ERenderTargetTextureType pRTTextureType,
			const RenderTargetTextureLayout & pRTTextureLayout,
			TextureReference pTargetTexture )
	: GPUResourceView( pGPUDevice, EGPUResourceBaseType::Texture, pTargetTexture->mTextureProperties.resourceFlags )
	, mRTTextureType( pRTTextureType )
	, mRTBufferMask( cxdefs::getRTBufferMaskForRenderTargetTextureType( pRTTextureType ) )
	, mRTTextureLayout( pRTTextureLayout )
	, mTargetTexture( pTargetTexture )
	{}

	RenderTargetTexture::RenderTargetTexture(
			GPUDevice & pGPUDevice,
			ERenderTargetTextureType pRTTextureType,
			const RenderTargetTextureLayout & pRTTextureLayout,
			GpaHandle<GPUDeviceChildObject> pInternalRenderBuffer,
			Bitmask<resource_flags_value_t> pRenderBufferFlags )
	: GPUResourceView( pGPUDevice, EGPUResourceBaseType::Texture, pRenderBufferFlags )
	, mRTTextureType( pRTTextureType )
	, mRTBufferMask( cxdefs::getRTBufferMaskForRenderTargetTextureType( pRTTextureType ) )
	, mRTTextureLayout( pRTTextureLayout )
	, mTargetTexture()
	, _internalRenderBuffer( pInternalRenderBuffer )
	{}

	RenderTargetTexture::~RenderTargetTexture() = default;

	bool RenderTargetTexture::empty() const noexcept
	{
		return !mTargetTexture && !_internalRenderBuffer;
	}

	bool RenderTargetTexture::isDepthStencilTexture() const noexcept
	{
		return mRTBufferMask.isSetAnyOf( E_RENDER_TARGET_BUFFER_MASK_DEPTH_STENCIL );
	}

	bool RenderTargetTexture::isDepthStencilRenderBuffer() const noexcept
	{
		return _internalRenderBuffer && isDepthStencilTexture();
	}

	namespace rcutil
	{

		ERenderTargetTextureType queryRenderTargetTextureType( ETextureFormat pFormat )
		{
			const Bitmask<uint8> pixelFormatFlags = cxdefs::getTextureFormatFlags( pFormat );

			if( pixelFormatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_COMPRESSED_BIT ) )
			{
				return ERenderTargetTextureType::Unknown;
			}
			else if( pixelFormatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_DEPTH_STENCIL_BIT ) )
			{
				return ERenderTargetTextureType::RTDepthStencil;
			}
			else if( pixelFormatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_DEPTH_BIT ) )
			{
				return ERenderTargetTextureType::RTDepthOnly;
			}
			else if( pixelFormatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_STENCIL_BIT ) )
			{
				return ERenderTargetTextureType::RTStencilOnly;
			}
			else /* Not a depth/stencil, not a compressed, should be a color-compatible one. */
			{
				return ERenderTargetTextureType::RTColor;
			}
		}

		RenderTargetTextureLayout queryRenderTargetTextureLayout( const TextureLayout & pTextureLayout )
		{
			RenderTargetTextureLayout rtTextureLayout{};
			rtTextureLayout.imageRect.width = pTextureLayout.dimensions.width;
			rtTextureLayout.imageRect.height = pTextureLayout.dimensions.height;
			rtTextureLayout.internalFormat = pTextureLayout.internalFormat;
			rtTextureLayout.msaaLevel = pTextureLayout.msaaLevel;

			return rtTextureLayout;
		}

		bool validateRenderTargetTextureLayout( TextureHandle pTargetTexture, const RenderTargetTextureLayout & pRTTextureLayout )
		{
			if( !pTargetTexture )
			{
				return false;
			}

			const auto & targetTextureLayout = pTargetTexture->mTextureLayout;

			const auto layoutMatch =
				( pRTTextureLayout.imageRect.width == targetTextureLayout.dimensions.width ) &&
				( pRTTextureLayout.imageRect.height == targetTextureLayout.dimensions.height ) &&
				( pRTTextureLayout.internalFormat == targetTextureLayout.internalFormat ) &&
				( pRTTextureLayout.msaaLevel == targetTextureLayout.msaaLevel );

			return layoutMatch;
		}

	}

}
