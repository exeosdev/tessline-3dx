
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_STATE_OBJECT_H__

#include "GL_pipelineStateDesc.h"
#include <ts3/gpuapi/state/renderTargetStateObject.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLFramebufferObject );
	ts3GLDeclareOpenGLObjectHandle( GLRenderbufferObject );

	class GLRenderTargetStateObject : public RenderTargetStateObject
	{
	public:
		GLFramebufferObjectHandle const mGLFramebufferObject;

	public:
		GLRenderTargetStateObject( GLGPUDevice & pGPUDevice,
		                           const RenderTargetLayout & pRTLayout,
		                           const RenderTargetResourceBinding & pRTResourceBinding,
		                           GLFramebufferObjectHandle pGLFramebufferObject );

		virtual ~GLRenderTargetStateObject();

		static GpaHandle<GLRenderTargetStateObject> create( GLGPUDevice & pGPUDevice,
		                                                    const RenderTargetStateObjectCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_STATE_OBJECT_H__
