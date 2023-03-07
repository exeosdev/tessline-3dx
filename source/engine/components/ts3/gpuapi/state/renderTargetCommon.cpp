
#include "renderTargetCommon.h"
#include <ts3/gpuapi/resources/texture.h>
#include <ts3/gpuapi/resources/renderTargetTexture.h>
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	namespace smutil
	{

		RenderTargetLayout getRenderTargetLayoutForBindingDefinition(
				const RenderTargetBindingDefinition & pBindingDefinition )
		{
			RenderTargetLayout rtLayout;
			rtLayout.activeAttachmentsMask = 0;

			for( uint32 colorAttachmentIndex = 0; colorAttachmentIndex < cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM; ++colorAttachmentIndex )
			{
				const auto attachmentBit = cxdefs::makeRTAttachmentFlag( colorAttachmentIndex );
				if( pBindingDefinition.activeAttachmentsMask.isSet( attachmentBit ) )
				{
					const auto & attachmentBinding = pBindingDefinition.colorAttachments[colorAttachmentIndex];
					if( !attachmentBinding.empty() )
					{
						const auto attachmentFormat = attachmentBinding.attachmentTexture->mRTTextureLayout.internalDataFormat;
						rtLayout.colorAttachments[colorAttachmentIndex].format = attachmentFormat;
						rtLayout.activeAttachmentsMask.set( attachmentBit );
					}
				}
			}

			if( pBindingDefinition.activeAttachmentsMask.isSet( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT ) )
			{
				if( !pBindingDefinition.depthStencilAttachment.empty() )
				{
					const auto attachmentFormat = pBindingDefinition.depthStencilAttachment.attachmentTexture->mRTTextureLayout.internalDataFormat;
					rtLayout.depthStencilAttachment.format = attachmentFormat;
					rtLayout.activeAttachmentsMask.set( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT );
				}
			}

			return rtLayout;
		}
	}

	namespace defaults
	{

		const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8 =
		{
			E_RT_ATTACHMENT_FLAG_COLOR_0_BIT,
			{
				RenderTargetAttachmentLayout { ETextureFormat::BGRA8UN }
			}
		};

		const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8D24S8 =
		{
			E_RT_ATTACHMENT_FLAGS_DEFAULT_C0_DS,
			{
				RenderTargetAttachmentLayout { ETextureFormat::BGRA8UN },
			},
			RenderTargetAttachmentLayout { ETextureFormat::D24UNS8U }
		};

		const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8 =
		{
			E_RT_ATTACHMENT_FLAG_COLOR_0_BIT,
			{
				RenderTargetAttachmentLayout { ETextureFormat::RGBA8UN }
			}
		};

		const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8D24S8 =
		{
			E_RT_ATTACHMENT_FLAGS_DEFAULT_C0_DS,
			{
				RenderTargetAttachmentLayout { ETextureFormat::RGBA8UN },
			},
			RenderTargetAttachmentLayout { ETextureFormat::D24UNS8U }
		};
	
	}


//	bool createRenderTargetLayout( const RenderTargetLayoutDesc & pRTLayoutDesc,
//	                               RenderTargetLayout & pOutRTLayout )
//	{
//		RenderTargetLayout rtLayout;
//
//		for( const auto & attachmentLayoutDesc : pRTLayoutDesc.attachmentLayoutDescArray )
//		{
//			if( attachmentLayoutDesc )
//			{
//				auto attachmentIndex = static_cast<uint32>( attachmentLayoutDesc.attachmentID );
//				if( attachmentIndex < cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM )
//				{
//					rtLayout.colorAttachmentArray[attachmentIndex].format = attachmentLayoutDesc.format;
//					rtLayout.colorAttachmentActiveCount += 1;
//					rtLayout.attachmentMask.set( E_RT_ATTACHMENT_FLAG_COLOR_0_BIT << attachmentIndex );
//				}
//				else
//				{
//					rtLayout.depthStencilAttachment.format = attachmentLayoutDesc.format;
//					rtLayout.attachmentMask.set( E_RENDER_TARGET_ATTACHMENT_FLAGS_DEPTH_STENCIL );
//				}
//			}
//		}
//
//		if( rtLayout.attachmentMask == 0 )
//		{
//			return false;
//		}
//
//		memCopy( pOutRTLayout, rtLayout );
//
//		return true;
//	}
//
//	RenderTargetLayout createRenderTargetLayout( const RenderTargetLayoutDesc & pRTLayoutDesc )
//	{
//		RenderTargetLayout renderTargetLayout;
//		createRenderTargetLayout( pRTLayoutDesc, renderTargetLayout );
//		return renderTargetLayout;
//	}
//
//	static bool validateRenderBufferRef( RenderTargetResourceBinding & pRTResourceBinding, const RTARenderBufferRef & pRenderBufferRef );
//	static bool validateTextureRef( RenderTargetResourceBinding & pRTResourceBinding, const RTATextureRef & pTextureRef );
//
//	bool createRenderTargetLayoutAndResourceBinding( const RenderTargetResourceBindingDesc & pRTResourceBindingDesc,
//	                                                 RenderTargetLayout & pOutRTLayout,
//	                                                 RenderTargetResourceBinding & pOutRTResourceBinding )
//	{
//		RenderTargetLayout rtLayout;
//		RenderTargetResourceBinding rtResourceBinding;
//
//		for( const auto & attachmentResourceBindingDesc : pRTResourceBindingDesc.attachmentResourceBindingDescArray )
//		{
//			if( rtLayout.colorAttachmentActiveCount + rtLayout.depthStencilAttachmentState == pRTResourceBindingDesc.activeBindingsNum )
//			{
//				break;
//			}
//
//			if( attachmentResourceBindingDesc )
//			{
//				auto attachmentIndex = static_cast<uint32>( attachmentResourceBindingDesc.attachmentID );
//				if( attachmentIndex >= cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM )
//				{
//					ts3DebugInterrupt();
//					return false;
//				}
//
//				RenderTargetAttachmentLayout * attachmentLayoutPtr = nullptr;
//				RenderTargetAttachmentResourceBinding * attachmentResourceBindingPtr = nullptr;
//
//				if( attachmentIndex < cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM )
//				{
//					attachmentLayoutPtr = &( rtLayout.colorAttachmentArray[attachmentIndex] );
//					attachmentResourceBindingPtr = &( rtResourceBinding.colorAttachmentArray[attachmentIndex] );
//					rtLayout.colorAttachmentActiveCount += 1;
//					rtLayout.attachmentMask.set( E_RT_ATTACHMENT_FLAG_COLOR_0_BIT << attachmentIndex );
//				}
//				else
//				{
//					attachmentLayoutPtr = &( rtLayout.depthStencilAttachment );
//					attachmentResourceBindingPtr = &( rtResourceBinding.depthStencilAttachment );
//					rtLayout.attachmentMask.set( E_RENDER_TARGET_ATTACHMENT_FLAGS_DEPTH_STENCIL );
//				}
//
//				if( attachmentResourceBindingDesc.attachmentResourceType == ERenderTargetResourceType::RenderBuffer )
//				{
//					if( !validateRenderBufferRef( rtResourceBinding, attachmentResourceBindingDesc.renderBufferRef ) )
//					{
//						return false;
//					}
//
//					auto * renderBuffer = attachmentResourceBindingDesc.renderBufferRef.renderBuffer;
//					attachmentLayoutPtr->format = renderBuffer->mRenderBufferLayout.internalDataFormat;
//					attachmentResourceBindingPtr->attachmentResourceType = ERenderTargetResourceType::RenderBuffer;
//					attachmentResourceBindingPtr->format = attachmentLayoutPtr->format;
//					attachmentResourceBindingPtr->uRenderBufferRef.renderBuffer = renderBuffer;
//				}
//				else
//				{
//					if( !validateTextureRef( rtResourceBinding, attachmentResourceBindingDesc.textureRef ) )
//					{
//						return false;
//					}
//
//					auto * texture = attachmentResourceBindingDesc.textureRef.texture;
//					attachmentLayoutPtr->format = texture->mTextureLayout.pixelFormat;
//					attachmentResourceBindingPtr->attachmentResourceType = ERenderTargetResourceType::Texture;
//					attachmentResourceBindingPtr->format = attachmentLayoutPtr->format;
//					attachmentResourceBindingPtr->uTextureRef.texture = texture;
//					attachmentResourceBindingPtr->uTextureRef.textureSubResource = attachmentResourceBindingDesc.textureRef.textureSubResource;
//				}
//			}
//		}
//
//		if( rtLayout.attachmentMask == 0 )
//		{
//			return false;
//		}
//
//		memCopy( pOutRTLayout, rtLayout );
//		memCopy( pOutRTResourceBinding, rtResourceBinding );
//
//		return true;
//	}
//
//	bool checkRenderTargetLayoutCompatibility( const RenderTargetResourceBinding & pRTResourceBinding,
//	                                           const RenderTargetLayout & pRTLayout )
//	{
//
//		for( uint32 colorAttachmentIndex = 0; colorAttachmentIndex < cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM; ++colorAttachmentIndex )
//		{
//			auto & caResourceBinding = pRTResourceBinding.colorAttachmentArray[colorAttachmentIndex];
//			auto & caLayout = pRTLayout.colorAttachmentArray[colorAttachmentIndex];
//
//			if( ( caResourceBinding && !caLayout ) || ( !caResourceBinding && caLayout ) )
//			{
//				return false;
//			}
//
//			if( caResourceBinding && caLayout )
//			{
//				if( caResourceBinding.format != caLayout.format )
//				{
//					return false;
//				}
//			}
//		}
//
//		auto & dsaResourceBinding = pRTResourceBinding.depthStencilAttachment;
//		auto & dsaLayout = pRTLayout.depthStencilAttachment;
//
//		if( ( dsaResourceBinding && !dsaLayout ) || ( !dsaResourceBinding && dsaLayout ) )
//		{
//			return false;
//		}
//
//		if( dsaResourceBinding && dsaLayout )
//		{
//			if( dsaResourceBinding.format != dsaLayout.format )
//			{
//				return false;
//			}
//		}
//
//		return true;
//	}
//
//
//	bool validateRenderBufferRef( RenderTargetResourceBinding & pRTResourceBinding, const RTARenderBufferRef & pRenderBufferRef )
//	{
//		if( !pRenderBufferRef.renderBuffer )
//		{
//			return false;
//		}
//
//		if( pRTResourceBinding.commonMSAALevel == CX_RT_BUFFER_MSAA_LEVEL_INVALID )
//		{
//			pRTResourceBinding.commonBufferSize = pRenderBufferRef.renderBuffer->mRenderBufferLayout.bufferSize;
//			pRTResourceBinding.commonMSAALevel = pRenderBufferRef.renderBuffer->mRenderBufferLayout.msaaLevel;
//
//			return true;
//		}
//
//		if( pRenderBufferRef.renderBuffer->mRenderBufferLayout.msaaLevel != pRTResourceBinding.commonMSAALevel )
//		{
//			return false;
//		}
//
//		if( ( pRenderBufferRef.renderBuffer->mRenderBufferLayout.bufferSize.width != pRTResourceBinding.commonBufferSize.width ) ||
//		    ( pRenderBufferRef.renderBuffer->mRenderBufferLayout.bufferSize.height != pRTResourceBinding.commonBufferSize.height ) )
//		{
//			return false;
//		}
//
//		return true;
//	}
//
//	bool validateTextureRef( RenderTargetResourceBinding & pRTResourceBinding, const RTATextureRef & pTextureRef )
//	{
//		if( !pTextureRef.texture )
//		{
//			return false;
//		}
//
//		const auto & textureLayout = pTextureRef.texture->mTextureLayout;
//
//		if( pRTResourceBinding.commonMSAALevel == CX_RT_BUFFER_MSAA_LEVEL_INVALID )
//		{
//			pRTResourceBinding.commonBufferSize.width = textureLayout.dimensions.width;
//			pRTResourceBinding.commonBufferSize.height = textureLayout.dimensions.height;
//			pRTResourceBinding.commonMSAALevel = textureLayout.msaaLevel;
//
//			return true;
//		}
//
//		if( textureLayout.msaaLevel != pRTResourceBinding.commonMSAALevel )
//		{
//			return false;
//		}
//
//		if( ( textureLayout.dimensions.width != pRTResourceBinding.commonBufferSize.width ) ||
//		    ( textureLayout.dimensions.height != pRTResourceBinding.commonBufferSize.height ) )
//		{
//			return false;
//		}
//
//		if( !TextureUtils::validateTextureSubResource( textureLayout.texClass, textureLayout.dimensions, pTextureRef.textureSubResource ) )
//		{
//			return false;
//		}
//
//		return true;
//	}

} // namespace ts3::gpuapi
