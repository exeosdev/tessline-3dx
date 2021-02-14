
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
		TextureLayout const mTextureLayout;
		TextureProperties const mTextureProperties;

		Texture( GPUDevice & pGPUDevice,
		         const ResourceMemoryInfo & pResourceMemory,
		         const TextureProperties & pTextureProperties,
		         const TextureLayout & pTextureLayout );

		virtual ~Texture();

		bool checkETextureTargetSupport( ETextureTarget pTextureTarget ) const;

	protected:
		static bool validateTextureCreateInfo( TextureCreateInfo & pCreateInfo );
	};

}

#endif // __TS3_GPUAPI_TEXTURE_H__
