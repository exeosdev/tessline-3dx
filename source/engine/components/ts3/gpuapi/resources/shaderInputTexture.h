
#pragma once

#ifndef __TS3_GPUAPI_SHADER_INPUT_TEXTURE_H__
#define __TS3_GPUAPI_SHADER_INPUT_TEXTURE_H__

#include "gpuResource.h"
#include "textureReference.h"

namespace ts3::gpuapi
{

	/// @brief
	struct ShaderInputTextureCreateInfo
	{
		TextureReference targetTexture;
		ShaderInputTextureLayout siTextureLayout;
		Bitmask<ETextureBindFlags> bindFlags;
	};

	/// @brief
	class ShaderInputTexture : public GPUResourceView
	{
		friend class GPUDevice;
		friend class Texture;

	public:
		ShaderInputTextureLayout const mSITextureLayout;

		/// Internal texture used by this SIT.
		TextureReference const mTargetTexture;

	public:
		ShaderInputTexture(
				GPUDevice & pGPUDevice,
				const ShaderInputTextureLayout & pSITextureLayout,
				TextureReference pTargetTexture );

		virtual ~ShaderInputTexture();

		///
		TS3_ATTR_NO_DISCARD bool empty() const noexcept;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SHADER_INPUT_TEXTURE_H__
