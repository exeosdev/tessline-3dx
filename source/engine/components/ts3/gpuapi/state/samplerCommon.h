
#pragma once

#ifndef __TS3_GPUAPI_SAMPLER_COMMON_H__
#define __TS3_GPUAPI_SAMPLER_COMMON_H__

#include "commonGraphicsConfig.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( Sampler );

	enum class ETextureAddressMode : enum_default_value_t
	{
		Undefined,
		ClampToColor,
		ClampToEdge,
		MirrorClampToEdge,
		MirrorRepeat,
		Repeat,
	};

	enum class ETextureCompareMode : enum_default_value_t
	{
		Undefined,
		None,
		RefToTexture,
	};

	enum class ETextureBorderPredefinedColor : enum_default_value_t
	{
		Undefined,
		OpaqueBlack,
		OpaqueWhite,
		TransparentBlack,
	};

	enum class ETextureFilter : enum_default_value_t
	{
		Undefined,
		Point,
		Linear,
		Anisotropic,
	};

	enum class ETextureMipMode : enum_default_value_t
	{
		Undefined,
		Disable,
		Nearest,
		Linear,
	};

	struct TextureCoordAddressModeConfig
	{
		ETextureAddressMode coordU;
		ETextureAddressMode coordV;
		ETextureAddressMode coordW;
	};

	struct TextureFilterConfig
	{
		uint32 anisotropyLevel;
		ETextureFilter magFilter;
		ETextureFilter minFilter;
		ETextureMipMode mipMode;
	};

	struct SamplerConfig
	{
		using MipLODRange = InclusiveRange<float>;
		TextureCoordAddressModeConfig addressModeConfig;
		math::RGBAColorR32Norm borderColor;
		ETextureBorderPredefinedColor borderPredefinedColor;
		TextureFilterConfig filterConfig;
		MipLODRange mipLODRange;
		float mipLODBias;
		ETextureCompareMode textureCompareMode;
		ECompFunc textureCompareFunc;
	};

	struct SamplerCreateInfo
	{
		SamplerConfig samplerConfig;
	};


	TS3_GPUAPI_OBJ const SamplerConfig cvSamplerConfigDefault;

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SAMPLER_COMMON_H__
