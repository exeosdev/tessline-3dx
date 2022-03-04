
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_BUFFER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_BUFFER_H__

#include <ts3/gpuapi/resources/renderBuffer.h>
#include <ts3/gpuapiGL/objects/GL_renderbufferObject.h>
#include <ts3/gpuapiGL/objects/GL_textureObject.h>

namespace ts3
{
namespace gpuapi
{

	class GLRenderBuffer : public RenderBuffer
	{
	public:
		GLRenderbufferObjectHandle const mGLRenderbufferObject;
		GLTextureObjectHandle const mGLTextureObject;

	public:
		GLRenderBuffer( GLGPUDevice & pGLGPUDevice,
		                ERenderBufferType pRenderBufferType,
		                const RenderBufferLayout & pRenderBufferLayout,
		                GLRenderbufferObjectHandle pGLRenderbufferObject );

		GLRenderBuffer( GLGPUDevice & pGLGPUDevice,
		                ERenderBufferType pRenderBufferType,
		                const RenderBufferLayout & pRenderBufferLayout,
		                GLTextureObjectHandle pGLTextureObject );

		virtual ~GLRenderBuffer();

		virtual bool isNull() const override;

		static Handle<GLRenderBuffer> create( GLGPUDevice & pGLGPUDevice, const RenderBufferCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_BUFFER_H__
