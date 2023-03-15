
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_BUFFER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_BUFFER_H__

#include <ts3/gpuapi/resources/renderTargetTexture.h>
#include <ts3/gpuapiGL/objects/GL_renderbufferObject.h>
#include <ts3/gpuapiGL/objects/GL_textureObject.h>

namespace ts3::gpuapi
{

	class GLInternalRenderBuffer : public GPUDeviceChildObject
	{
	public:
		GLRenderbufferObjectHandle const mGLRenderbufferObject;
		GLTextureObjectHandle const mGLTextureObject;

	public:
		GLInternalRenderBuffer(
				GLGPUDevice & pGPUDevice,
				GLRenderbufferObjectHandle pGLRenderbufferObject );

		GLInternalRenderBuffer(
				GLGPUDevice & pGPUDevice,
				GLTextureObjectHandle pGLTextureObject );

		static GpaHandle<GLInternalRenderBuffer> createInstance(
				GLGPUDevice & pGPUDevice,
				const RenderTargetTextureCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_BUFFER_H__
