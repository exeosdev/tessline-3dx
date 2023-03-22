
#include "GL_commonStateDefs.h"

namespace ts3::gpuapi
{

	namespace smutil
	{

		GLenum getFramebufferDepthStencilAttachmentIDForRTBufferMask( Bitmask<ERenderTargetBufferFlags> pRTBufferMask )
		{
			GLenum framebufferAttachmentID = 0;

			if( pRTBufferMask.isSet( E_RENDER_TARGET_BUFFER_MASK_DEPTH_STENCIL ) )
			{
				framebufferAttachmentID = GL_DEPTH_STENCIL_ATTACHMENT;
			}
			else if( pRTBufferMask.isSet( E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT ) )
			{
				framebufferAttachmentID = GL_DEPTH_ATTACHMENT;
			}
			else if( pRTBufferMask.isSet( E_RENDER_TARGET_BUFFER_FLAG_STENCIL_BIT ) )
			{
				framebufferAttachmentID = GL_STENCIL_ATTACHMENT;
			}

			return framebufferAttachmentID;
		}

	}

}
