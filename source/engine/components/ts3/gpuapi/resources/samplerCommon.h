
#pragma once

#ifndef __TS3_GPUAPI_SAMPLER_COMMON_H__
#define __TS3_GPUAPI_SAMPLER_COMMON_H__

#include "commonGPUResourceDefs.h"
#include "../state/commonGraphicsConfig.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( Sampler );

	using ETextureCompareState = EActiveState;

	enum class ETextureAddressMode : enum_default_value_t
	{
		BorderColor,
		Clamp,
		Mirror,
		MirrorOnceClamp,
		Wrap,
	};

	enum class ETextureFilter : enum_default_value_t
	{
		Point,
		Linear,
		Anisotropic,
	};

	enum class ETextureMipMode : enum_default_value_t
	{
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

	struct ETextureFilterConfig
	{
		uint32 anisotropyLevel;
		ETextureFilter magFilter;
		ETextureFilter minFilter;
		ETextureMipMode mipMode;
	};

	struct SamplerDesc
	{
		using MipLODRange = InclusiveRange<float>;
		TextureCoordAddressModeConfig addressModeConfig;
		math::RGBAColorR32Norm borderColor;
		ETextureFilterConfig filterConfig;
		MipLODRange mipLODRange;
		float mipLODBias;
		ETextureCompareState textureCompareState;
		ECompFunc textureCompareFunc;
	};

	struct SamplerCreateInfo
	{
		SamplerDesc samplerDesc;
	};


	TS3_GPUAPI_OBJ const SamplerDesc cvSamplerDescDefault;

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SAMPLER_COMMON_H__
