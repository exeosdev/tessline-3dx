
#pragma once

#ifndef __TS3_GPUAPI_TEXTURE_H__
#define __TS3_GPUAPI_TEXTURE_H__

#include "gpuResource.h"
#include "textureCommon.h"

namespace ts3::gpuapi
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

		Texture( GPUDevice & pGPUDevice,
		         const ResourceMemoryInfo & pResourceMemory,
		         const TextureProperties & pTextureProperties,
		         const TextureLayout & pTextureLayout );

		virtual ~Texture();

		bool checkETextureTargetSupport( ETextureTarget pTextureTarget ) const;

	protected:
		static bool validateTextureCreateInfo( TextureCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_TEXTURE_H__
