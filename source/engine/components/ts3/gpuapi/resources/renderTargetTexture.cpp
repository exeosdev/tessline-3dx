
#include "renderTargetTexture.h"
#include "texture.h"

namespace ts3::gpuapi
{

	RenderTargetTexture::RenderTargetTexture(
			GPUDevice & pGPUDevice,
			ERenderTargetTextureType pRTTextureType,
			const RenderTargetTextureLayout & pRTTextureLayout,
			TextureReference pTargetTexture )
	: GPUResourceWrapper( pGPUDevice, EGPUResourceBaseType::Texture )
	, mRTTextureType( pRTTextureType )
	, mRTTextureLayout( pRTTextureLayout )
	, mInternalResourceFlags( pTargetTexture->mTextureProperties.resourceFlags )
	, _targetTexture( pTargetTexture )
	{}

	RenderTargetTexture::RenderTargetTexture(
			GPUDevice & pGPUDevice,
			ERenderTargetTextureType pRTTextureType,
			const RenderTargetTextureLayout & pRTTextureLayout,
			GpaHandle<GPUDeviceChildObject> pInternalRenderBuffer,
			Bitmask<resource_flags_value_t> pRenderBufferFlags )
	: GPUResourceWrapper( pGPUDevice, EGPUResourceBaseType::Texture )
	, mRTTextureType( pRTTextureType )
	, mRTTextureLayout( pRTTextureLayout )
	, mInternalResourceFlags( pRenderBufferFlags )
	, _internalRenderBuffer( pInternalRenderBuffer )
	{}

	RenderTargetTexture::~RenderTargetTexture() = default;

	const TextureReference & RenderTargetTexture::getTargetTextureRef() const noexcept
	{
		return _targetTexture;
	}

	GPUDeviceChildObject * RenderTargetTexture::getInternalRenderBuffer() const noexcept
	{
		return _internalRenderBuffer.get();
	}

	bool RenderTargetTexture::isDepthStencilTexture() const noexcept
	{
		return ( mRTTextureType == ERenderTargetTextureType::RTDepth ) ||
		       ( mRTTextureType == ERenderTargetTextureType::RTDepthStencil );
	}

	bool RenderTargetTexture::isDepthStencilRenderBuffer() const noexcept
	{
		return _targetTexture.empty() && isDepthStencilTexture();
	}

	void RenderTargetTexture::setTargetTexture( const TextureReference & pTargetTextureRef )
	{
		_targetTexture = pTargetTextureRef;
	}

	void RenderTargetTexture::setInternalRenderBuffer( GpaHandle<GPUDeviceChildObject> pInternalRenderBuffer )
	{
		_internalRenderBuffer = pInternalRenderBuffer;
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

		RenderTargetTextureLayout queryRenderTargetTextureLayout( const TextureLayout & pTextureLayout )
		{
			RenderTargetTextureLayout rtTextureLayout{};
			rtTextureLayout.bufferSize.width = pTextureLayout.dimensions.width;
			rtTextureLayout.bufferSize.height = pTextureLayout.dimensions.height;
			rtTextureLayout.internalDataFormat = pTextureLayout.pixelFormat;
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
				( pRTTextureLayout.bufferSize.width == targetTextureLayout.dimensions.width ) &&
				( pRTTextureLayout.bufferSize.height == targetTextureLayout.dimensions.height ) &&
				( pRTTextureLayout.internalDataFormat == targetTextureLayout.pixelFormat ) &&
				( pRTTextureLayout.msaaLevel == targetTextureLayout.msaaLevel );

			return layoutMatch;
		}

	}

}
