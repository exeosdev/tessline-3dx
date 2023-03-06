
#include "renderTargetTexture.h"

namespace ts3::gpuapi
{

	RenderTargetTexture::RenderTargetTexture(
			GPUDevice & pGPUDevice,
			ERenderTargetTextureType pRTTextureType,
			const RenderTargetTextureLayout & pRTTextureLayout )
	: GPUResource( pGPUDevice, EGPUResourceBaseType::Texture, {} )
	{}

	RenderTargetTexture::RenderTargetTexture( TextureHandle pTargetTexture )
	{}

	RenderTargetTexture::~RenderTargetTexture() = default;

	TextureHandle RenderTargetTexture::getTargetTexture() const
	{
	}

	bool RenderTargetTexture::isDepthStencilRenderBuffer() const
	{
	}

	void RenderTargetTexture::setTargetTexture( TextureHandle pTargetTexture )
	{
		_targetTexture = pTargetTexture;
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
				return ERenderTargetTextureType::RTDepth;
			}
			else /* Not a depth/stencil, not a compressed, should be a color-compatible one. */
			{
				ts3DebugAssert( rcutil::checkRenderTargetTextureColorFormat( pFormat ) );
				return ERenderTargetTextureType::RTColor;
			}
		}

	}

}
