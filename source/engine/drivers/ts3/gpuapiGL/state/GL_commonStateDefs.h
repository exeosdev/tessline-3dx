
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_STATE_DEFS_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_STATE_DEFS_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/renderTargetCommon.h>

namespace ts3::gpuapi
{

	namespace smutil
	{

		TS3_ATTR_NO_DISCARD GLenum getFramebufferDepthStencilAttachmentIDForRTBufferMask( Bitmask<ERenderTargetBufferFlags> pRTBufferMask );

	}

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_STATE_DEFS_H__
