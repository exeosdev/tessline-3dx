
#pragma once

#ifndef __TS3_GPUAPI_TEXTURE_H__
#define __TS3_GPUAPI_TEXTURE_H__

#include "gpuResource.h"
#include "textureCommon.h"

namespace ts3::GpuAPI
{

	class TextureStorage;

	struct TextureProperties : public GPUResourceProperties
	{
	};

	class Texture : public GPUResource
	{
	public:
        TextureProperties const mTextureProperties;
		TextureLayout const mTextureLayout;

		Texture(
			GPUDevice & pGPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const TextureProperties & pTextureProperties,
			const TextureLayout & pTextureLayout );

		virtual ~Texture();

		TS3_ATTR_NO_DISCARD virtual const GPUResourceProperties & getProperties() const override final;

		TS3_ATTR_NO_DISCARD bool checkTextureTargetSupport( ETextureTarget pTextureTarget ) const;

	protected:
		static bool validateTextureCreateInfo( TextureCreateInfo & pCreateInfo );
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_TEXTURE_H__
