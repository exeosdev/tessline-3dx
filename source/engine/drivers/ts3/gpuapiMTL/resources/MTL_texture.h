
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_TEXTURE_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_TEXTURE_H__

#include "../MTL_prerequisites.h"
#include <ts3/gpuapi/resources/texture.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( MetalTexture );

	class MetalTexture : public Texture
	{
		friend class MetalCommandContext;

	public:
		MetalTextureObjectHandle const mMetalTextureObject = nullptr;

	public:
		MetalTexture( MetalGPUDevice & pGPUDevice,
		           const ResourceMemoryInfo & pResourceMemory,
		           const TextureProperties & pTextureProperties,
		           const TextureLayout & pTextureLayout,
		           MetalTextureObjectHandle pMetalTextureObject );

		virtual ~MetalTexture();

		static MetalTextureHandle create( MetalGPUDevice & pGPUDevice, const TextureCreateInfo & pCreateInfo );

		static RenderTargetTextureHandle createForRenderTarget( MetalGPUDevice & pGPUDevice, const RenderTargetTextureCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_TEXTURE_H__
