
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
		GLTexture( GLGPUDevice & pGLGPUDevice,
		           const ResourceMemoryInfo & pResourceMemory,
		           const TextureProperties & pTextureProperties,
		           const TextureLayout & pTextureLayout,
		           GLTextureObjectHandle pGLTextureObject );

		virtual ~GLTexture();

		static GLTextureHandle create( GLGPUDevice & pGLGPUDevice, const TextureCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_TEXTURE_H__
