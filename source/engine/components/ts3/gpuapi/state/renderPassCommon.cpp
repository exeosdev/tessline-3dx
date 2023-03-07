
#include "renderPassCommon.h"

namespace ts3::gpuapi
{

	namespace smutil
	{

		Bitmask<ERenderPassActionFlags> getRenderPassAttachmentActionMask(
				const RenderPassAttachmentUsage & pAttachmentUsage )
		{
			Bitmask<ERenderPassActionFlags> attachmentActionMask = 0;
			attachmentActionMask.set( static_cast<uint32>( pAttachmentUsage.renderPassLoadAction ) );
			attachmentActionMask.set( static_cast<uint32>( pAttachmentUsage.renderPassStoreAction ) );
			return attachmentActionMask;
		}

	}

}
