
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_STATE_OBJECT_H__
#define __TS3_GPUAPI_RENDER_TARGET_STATE_OBJECT_H__

#include "gpuStateObject.h"
#include "renderTargetCommon.h"
#include <ts3/gpuapi/resources/renderBuffer.h>

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( RenderBuffer );
	ts3DeclareClassHandle( Texture );

	struct RenderTargetStateObjectCreateInfo
	{
		RenderTargetResourceBindingDesc rtResourceBindingDesc;
	};

	class RenderTargetStateObject : public GPUStateObject
	{
	public:
		RenderTargetLayout const mRTLayout;
		RenderTargetResourceBinding const mRTResourceBinding;

	public:
		RenderTargetStateObject( GPUDevice & pGPUDevice,
		                         const RenderTargetLayout & pRTLayout,
		                         const RenderTargetResourceBinding & pRTResourceBinding );

		virtual ~RenderTargetStateObject();
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_RENDER_TARGET_STATE_OBJECT_H__
