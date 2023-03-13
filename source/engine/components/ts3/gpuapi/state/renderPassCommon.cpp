
#include "renderPassCommon.h"

namespace ts3::gpuapi
{

	void RenderPassConfiguration::resetAttachmentsFlags() noexcept
	{
		attachmentsAccessRestrictMask = 0;
		attachmentsActionClearMask = 0;
		attachmentsActionResolveMask = 0;

		foreachRTAttachmentIndex( activeAttachmentsMask,
			[&]( native_uint pIndex, ERTAttachmentFlags pAttachmentBit )
			{
				const auto & attachmentConfig = attachments[pIndex];
				if( !attachmentConfig )
				{
					activeAttachmentsMask.unset( pAttachmentBit );
				}
				else
				{
					const auto attachmentActionMask = smutil::getRenderPassAttachmentActionMask( attachmentConfig );
					if( attachmentActionMask.isSet( E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_ACCESS_RESTRICT_BIT ) )
					{
						attachmentsAccessRestrictMask.set( pAttachmentBit );
					}
					else
					{
						if( attachmentActionMask.isSet( E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_CLEAR_BIT ) )
						{
							attachmentsActionClearMask.set( pAttachmentBit );
						}
						if( attachmentActionMask.isSet( E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESOLVE_BIT ) )
						{
							attachmentsActionResolveMask.set( pAttachmentBit );
						}
					}
				}
				return true;
			} );
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
			foreachRTAttachmentIndex( pActiveAttachmentsMask,
				[&]( native_uint pIndex, ERTAttachmentFlags pAttachmentBit )
				{
					const auto attachmentUsage = getRenderPassAttachmentActionMask( pColorAttachments[pIndex] );
					if( attachmentUsage.isSet( pActionMask ) )
					{
						attachmentActionMask.set( pAttachmentBit );
					}
					return true;
				} );
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
			foreachRTAttachmentIndex( pRenderPassConfig.activeAttachmentsMask,
				[&]( native_uint pIndex, ERTAttachmentFlags pAttachmentBit )
				{
					const auto attachmentUsage = getRenderPassAttachmentActionMask( pRenderPassConfig.colorAttachments[pIndex] );
					if( attachmentUsage.isSet( E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_LOAD_CLEAR_BIT ) )
					{
						pRenderPassConfig.attachmentsActionClearMask.set( pAttachmentBit );
					}
					if( attachmentUsage.isSet( E_RENDER_PASS_ATTACHMENT_ACTION_FLAG_STORE_RESOLVE_BIT ) )
					{
						pRenderPassConfig.attachmentsActionResolveMask.set( pAttachmentBit );
					}
					return true;
				} );
		}

	}

}
