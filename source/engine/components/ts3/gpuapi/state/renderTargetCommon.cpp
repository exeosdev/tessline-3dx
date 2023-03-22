
#include "renderTargetCommon.h"
#include <ts3/gpuapi/resources/texture.h>
#include <ts3/gpuapi/resources/renderTargetTexture.h>
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	void RenderTargetBindingDefinition::resetAttachmentsFlags() noexcept
	{
		attachmentsActionResolveMask = 0;

		foreachRTAttachmentIndex( activeAttachmentsMask,
			[&]( native_uint pIndex, ERTAttachmentFlags pAttachmentBit )
			{
				const auto & attachmentBinding = attachments[pIndex];
				if( !attachmentBinding )
				{
					activeAttachmentsMask.unset( pAttachmentBit );
				}
				else
				{
					if( attachmentBinding.resolveTexture )
					{
						attachmentsActionResolveMask.set( pAttachmentBit );
					}
				}
				return true;
			} );
	}


	namespace smutil
	{

		const RenderTargetAttachmentBinding * getRenderTargetBindingDefinitionFirstTarget(
				const RenderTargetBindingDefinition & pBindingDefinition )
		{
			const RenderTargetAttachmentBinding * firstBinding = nullptr;

			for( const auto & attachmentBinding : pBindingDefinition.attachments )
			{
				if( attachmentBinding )
				{
					firstBinding = &attachmentBinding;
					break;
				}
			}

			return firstBinding;
		}

		bool validateRenderTargetBindingDefinition( const RenderTargetBindingDefinition & pBindingDefinition )
		{
			if( const auto * firstBinding = getRenderTargetBindingDefinitionFirstTarget( pBindingDefinition ) )
			{
				const auto & commonImageLayout = firstBinding->attachmentTexture->mRTTextureLayout;

				const auto bindingValid = foreachRTAttachmentIndex( pBindingDefinition.activeAttachmentsMask,
					[&]( native_uint pIndex, ERTAttachmentFlags pAttachmentBit )
					{
						const auto & attachmentBinding = pBindingDefinition.attachments[pIndex];
						if( !attachmentBinding )
						{
							return false;
						}
						const auto requiredUsageFlags = cxdefs::getRTAttachmentRequiredUsageMask( pIndex );
						if( !attachmentBinding.attachmentTexture->mResourceFlags.isSetAnyOf( requiredUsageFlags ) )
						{
							return false;
						}
						const auto & textureLayout = attachmentBinding.attachmentTexture->mRTTextureLayout;
						if( textureLayout.imageRect != commonImageLayout.imageRect )
						{
							return false;
						}
						if( textureLayout.msaaLevel != commonImageLayout.msaaLevel )
						{
							return false;
						}
						if( pBindingDefinition.attachmentsActionResolveMask.isSet( pAttachmentBit ) && !attachmentBinding.resolveTexture )
						{
							return false;
						}
						return true;
					} );

				if( bindingValid )
				{
					return true;
				}
			}

			return false;
		}

		RenderTargetLayout getRenderTargetLayoutForBindingDefinition(
				const RenderTargetBindingDefinition & pBindingDefinition )
		{
			if( const auto * firstBinding = getRenderTargetBindingDefinitionFirstTarget( pBindingDefinition ) )
			{
				const auto & commonLayout = firstBinding->attachmentTexture->mRTTextureLayout;

				RenderTargetLayout renderTargetLayout{};
				renderTargetLayout.activeAttachmentsMask = 0;
				renderTargetLayout.sharedImageRect = commonLayout.imageRect;
				renderTargetLayout.sharedMSAALevel = commonLayout.msaaLevel;

				const auto layoutValid = foreachRTAttachmentIndex( pBindingDefinition.activeAttachmentsMask,
					[&]( native_uint pIndex, ERTAttachmentFlags pAttachmentBit )
					{
						const auto & attachmentBinding = pBindingDefinition.attachments[pIndex];
						if( attachmentBinding.empty() )
						{
							return false;
						}

						const auto & caTexture = attachmentBinding.attachmentTexture;
						const auto & caLayout = caTexture->mRTTextureLayout;

						if( ( caLayout.imageRect != commonLayout.imageRect ) || ( caLayout.msaaLevel != commonLayout.msaaLevel ) )
						{
							return false;
						}

						renderTargetLayout.attachments[pIndex].format = caLayout.internalFormat;
						renderTargetLayout.activeAttachmentsMask.set( pAttachmentBit );

						return true;
					} );

				if( layoutValid )
				{
					return renderTargetLayout;
				}
			}

			return RenderTargetLayout{};
		}
	}

	namespace defaults
	{

		RenderTargetLayout getRenderTargetLayoutDefaultBGRA8()
		{
			RenderTargetLayout rtLayout{};
			rtLayout.activeAttachmentsMask = E_RT_ATTACHMENT_FLAG_COLOR_0_BIT;
			rtLayout.sharedImageRect = cxdefs::TEXTURE_SIZE_2D_UNDEFINED;
			rtLayout.sharedMSAALevel = cxdefs::TEXTURE_MSAA_LEVEL_UNDEFINED;
			rtLayout.colorAttachments[0].format = ETextureFormat::BGRA8UN;
			return rtLayout;
		}

		RenderTargetLayout getRenderTargetLayoutDefaultBGRA8D24S8()
		{
			RenderTargetLayout rtLayout{};
			rtLayout.activeAttachmentsMask = E_RT_ATTACHMENT_FLAG_COLOR_0_BIT | E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT;
			rtLayout.sharedImageRect = cxdefs::TEXTURE_SIZE_2D_UNDEFINED;
			rtLayout.sharedMSAALevel = cxdefs::TEXTURE_MSAA_LEVEL_UNDEFINED;
			rtLayout.colorAttachments[0].format = ETextureFormat::BGRA8UN;
			rtLayout.depthStencilAttachment.format = ETextureFormat::D24UNS8U;
			return rtLayout;
		}

		RenderTargetLayout getRenderTargetLayoutDefaultRGBA8()
		{
			RenderTargetLayout rtLayout{};
			rtLayout.activeAttachmentsMask = E_RT_ATTACHMENT_FLAG_COLOR_0_BIT;
			rtLayout.sharedImageRect = cxdefs::TEXTURE_SIZE_2D_UNDEFINED;
			rtLayout.sharedMSAALevel = cxdefs::TEXTURE_MSAA_LEVEL_UNDEFINED;
			rtLayout.colorAttachments[0].format = ETextureFormat::RGBA8UN;
			return rtLayout;
		}

		RenderTargetLayout getRenderTargetLayoutDefaultRGBA8D24S8()
		{
			RenderTargetLayout rtLayout{};
			rtLayout.activeAttachmentsMask = E_RT_ATTACHMENT_FLAG_COLOR_0_BIT | E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT;
			rtLayout.sharedImageRect = cxdefs::TEXTURE_SIZE_2D_UNDEFINED;
			rtLayout.sharedMSAALevel = cxdefs::TEXTURE_MSAA_LEVEL_UNDEFINED;
			rtLayout.colorAttachments[0].format = ETextureFormat::RGBA8UN;
			rtLayout.depthStencilAttachment.format = ETextureFormat::D24UNS8U;
			return rtLayout;
		}

		const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8 = getRenderTargetLayoutDefaultBGRA8();

		const RenderTargetLayout cvRenderTargetLayoutDefaultBGRA8D24S8 = getRenderTargetLayoutDefaultBGRA8D24S8();

		const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8 = getRenderTargetLayoutDefaultRGBA8();

		const RenderTargetLayout cvRenderTargetLayoutDefaultRGBA8D24S8 = getRenderTargetLayoutDefaultRGBA8D24S8();
	
	}


//	bool createRenderTargetLayout( const RenderTargetLayoutDesc & pRTLayoutDesc,
//	                               RenderTargetLayout & pOutRTLayout )
//	{
//		RenderTargetLayout renderTargetLayout;
//
//		for( const auto & attachmentLayoutDesc : pRTLayoutDesc.attachmentLayoutDescArray )
//		{
//			if( attachmentLayoutDesc )
//			{
//				auto attachmentIndex = static_cast<uint32>( attachmentLayoutDesc.attachmentID );
//				if( attachmentIndex < gpm::RT_MAX_COLOR_ATTACHMENTS_NUM )
//				{
//					renderTargetLayout.colorAttachmentArray[attachmentIndex].format = attachmentLayoutDesc.format;
//					renderTargetLayout.colorAttachmentActiveCount += 1;
//					renderTargetLayout.attachmentMask.set( E_RT_ATTACHMENT_FLAG_COLOR_0_BIT << attachmentIndex );
//				}
//				else
//				{
//					renderTargetLayout.depthStencilAttachment.format = attachmentLayoutDesc.format;
//					renderTargetLayout.attachmentMask.set( E_RENDER_TARGET_ATTACHMENT_FLAGS_DEPTH_STENCIL );
//				}
//			}
//		}
//
//		if( renderTargetLayout.attachmentMask == 0 )
//		{
//			return false;
//		}
//
//		memCopy( pOutRTLayout, renderTargetLayout );
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
//		RenderTargetLayout renderTargetLayout;
//		RenderTargetResourceBinding rtResourceBinding;
//
//		for( const auto & attachmentResourceBindingDesc : pRTResourceBindingDesc.attachmentResourceBindingDescArray )
//		{
//			if( renderTargetLayout.colorAttachmentActiveCount + renderTargetLayout.depthStencilAttachmentState == pRTResourceBindingDesc.activeBindingsNum )
//			{
//				break;
//			}
//
//			if( attachmentResourceBindingDesc )
//			{
//				auto attachmentIndex = static_cast<uint32>( attachmentResourceBindingDesc.attachmentID );
//				if( attachmentIndex >= gpm::RT_MAX_COMBINED_ATTACHMENTS_NUM )
//				{
//					ts3DebugInterrupt();
//					return false;
//				}
//
//				RenderTargetAttachmentLayout * attachmentLayoutPtr = nullptr;
//				RenderTargetAttachmentResourceBinding * attachmentResourceBindingPtr = nullptr;
//
//				if( attachmentIndex < gpm::RT_MAX_COLOR_ATTACHMENTS_NUM )
//				{
//					attachmentLayoutPtr = &( renderTargetLayout.colorAttachmentArray[attachmentIndex] );
//					attachmentResourceBindingPtr = &( rtResourceBinding.colorAttachmentArray[attachmentIndex] );
//					renderTargetLayout.colorAttachmentActiveCount += 1;
//					renderTargetLayout.attachmentMask.set( E_RT_ATTACHMENT_FLAG_COLOR_0_BIT << attachmentIndex );
//				}
//				else
//				{
//					attachmentLayoutPtr = &( renderTargetLayout.depthStencilAttachment );
//					attachmentResourceBindingPtr = &( rtResourceBinding.depthStencilAttachment );
//					renderTargetLayout.attachmentMask.set( E_RENDER_TARGET_ATTACHMENT_FLAGS_DEPTH_STENCIL );
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
//					attachmentLayoutPtr->format = renderBuffer->mRenderBufferLayout.internalFormat;
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
//		if( renderTargetLayout.attachmentMask == 0 )
//		{
//			return false;
//		}
//
//		memCopy( pOutRTLayout, renderTargetLayout );
//		memCopy( pOutRTResourceBinding, rtResourceBinding );
//
//		return true;
//	}
//
//	bool checkRenderTargetLayoutCompatibility( const RenderTargetResourceBinding & pRTResourceBinding,
//	                                           const RenderTargetLayout & pRTLayout )
//	{
//
//		for( uint32 caIndex = 0; caIndex < gpm::RT_MAX_COLOR_ATTACHMENTS_NUM; ++caIndex )
//		{
//			auto & caResourceBinding = pRTResourceBinding.colorAttachmentArray[caIndex];
//			auto & caLayout = pRTLayout.colorAttachmentArray[caIndex];
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
