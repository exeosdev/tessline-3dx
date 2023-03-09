
#include "renderPassCommon.h"

namespace ts3::gpuapi
{

	void RenderPassConfiguration::resetAttachmentsFlags() noexcept
	{
		attachmentActionResolveMask = 0;
		attachmentsAccessRestrictMask = 0;
		attachmentActionClearMask = 0;
		attachmentActionDiscardMask = 0;

		for(  uint32 caIndex = 0; caIndex < cxdefs::RT_MAX_COMBINED_ATTACHMENTS_NUM; ++caIndex )
		{
			const auto colorAttachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );
			if( activeAttachmentsMask.isSet( colorAttachmentBit ) )
			{
				const auto & attachmentConfig = colorAttachments[caIndex];
				if( !attachmentConfig )
				{
					activeAttachmentsMask.unset( colorAttachmentBit );
				}
				else
				{
				}
			}
		}
	}

	namespace smutil
	{

		Bitmask<ERenderPassAttachmentActionFlags> getRenderPassAttachmentActionMask(
				const RenderPassAttachmentConfig & pAttachmentConfig )
		{
			Bitmask<ERenderPassAttachmentActionFlags> attachmentActionMask = 0;
			attachmentActionMask.set( static_cast<uint32>( pAttachmentConfig.renderPassLoadAction ) );
			attachmentActionMask.set( static_cast<uint32>( pAttachmentConfig.renderPassStoreAction ) );
			return attachmentActionMask;
		}

		Bitmask<ERTAttachmentFlags> getRenderPassAttachmentArbitraryActionMask(
				const RenderPassColorAttachmentConfigArray & pColorAttachments,
				const RenderPassAttachmentConfig pDepthStencilAttachment,
				Bitmask<ERTAttachmentFlags> pActiveAttachmentsMask,
				Bitmask<ERenderPassAttachmentActionFlags> pActionMask )
		{
			Bitmask<ERTAttachmentFlags> attachmentActionMask = 0;
			for( uint32 caIndex = 0; caIndex < cxdefs::RT_MAX_COMBINED_ATTACHMENTS_NUM; ++caIndex )
			{
				const auto attachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );
				if( pActiveAttachmentsMask.isSet( attachmentBit ) )
				{
					const auto attachmentUsage = getRenderPassAttachmentActionMask( pColorAttachments[caIndex] );
					if( attachmentUsage.isSet( pActionMask ) )
					{
						attachmentActionMask.set( attachmentBit );
					}
				}
			}
			return attachmentActionMask;
		}

		Bitmask<ERTAttachmentFlags> getRenderPassAttachmentClearMask(
				const RenderPassColorAttachmentConfigArray & pColorAttachments,
				const RenderPassAttachmentConfig pDepthStencilAttachment,
				Bitmask<ERTAttachmentFlags> pActiveAttachmentsMask )
		{
			return getRenderPassAttachmentArbitraryActionMask(
					pColorAttachments,
					pDepthStencilAttachment,
					pActiveAttachmentsMask,
					E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_CLEAR_BIT );
		}

		Bitmask<ERTAttachmentFlags> getRenderPassAttachmentResolveMask(
				const RenderPassColorAttachmentConfigArray & pColorAttachments,
				const RenderPassAttachmentConfig pDepthStencilAttachment,
				Bitmask<ERTAttachmentFlags> pActiveAttachmentsMask )
		{
			return getRenderPassAttachmentArbitraryActionMask(
					pColorAttachments,
					pDepthStencilAttachment,
					pActiveAttachmentsMask,
					E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESOLVE_BIT );
		}

		void updateRenderPassConfigurationFlags( RenderPassConfiguration & pRenderPassConfig )
		{
			for( uint32 caIndex = 0; caIndex < cxdefs::RT_MAX_COMBINED_ATTACHMENTS_NUM; ++caIndex )
			{
				const auto attachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );
				if( pRenderPassConfig.activeAttachmentsMask.isSet( attachmentBit ) )
				{
					const auto attachmentUsage = getRenderPassAttachmentActionMask( pRenderPassConfig.colorAttachments[caIndex] );
					if( attachmentUsage.isSet( E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_CLEAR_BIT ) )
					{
						pRenderPassConfig.attachmentActionClearMask.set( attachmentBit );
					}
					if( attachmentUsage.isSet( E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESOLVE_BIT ) )
					{
						pRenderPassConfig.attachmentActionResolveMask.set( attachmentBit );
					}
				}
			}
		}

	}

}
