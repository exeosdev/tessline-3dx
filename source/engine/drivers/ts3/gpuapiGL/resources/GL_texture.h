
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_TEXTURE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_TEXTURE_H__

#include "../objects/GL_textureObject.h"
#include <ts3/gpuapi/resources/texture.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GLTexture );

	class GLTexture : public Texture
	{
		friend class GLCommandContext;

	public:
		GLTextureObjectHandle const mGLTextureObject = nullptr;

	public:
		GLTexture( GLGPUDevice & pGPUDevice,
		           const ResourceMemoryInfo & pResourceMemory,
		           const TextureProperties & pTextureProperties,
		           const TextureLayout & pTextureLayout,
		           GLTextureObjectHandle pGLTextureObject );

		virtual ~GLTexture();

		static GLTextureHandle createInstance( GLGPUDevice & pGPUDevice, const TextureCreateInfo & pCreateInfo );

		static RenderTargetTextureHandle createRTT( GLGPUDevice & pGPUDevice, const RenderTargetTextureCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_TEXTURE_H__
