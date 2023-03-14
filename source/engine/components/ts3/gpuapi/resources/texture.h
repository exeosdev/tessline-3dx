
#pragma once

#ifndef __TS3_GPUAPI_TEXTURE_H__
#define __TS3_GPUAPI_TEXTURE_H__

#include "gpuResource.h"
#include "textureReference.h"

namespace ts3::gpuapi
{

	struct TextureProperties : public GPUResourceProperties
	{
	};

	class Texture : public GPUResource
	{
		friend class CommandList;
		friend class TextureReference;

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

		TS3_ATTR_NO_DISCARD TextureSubResource getAllSubResourcesRef() const;

		TS3_ATTR_NO_DISCARD TextureSubResource getDefaultSubResourceRef() const;

	protected:
		static bool validateTextureCreateInfo( TextureCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_TEXTURE_H__
