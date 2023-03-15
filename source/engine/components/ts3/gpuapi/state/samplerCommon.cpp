
#include "samplerCommon.h"

namespace ts3::gpuapi
{

	const SamplerConfig cvSamplerConfigDefault =
	{
		TextureCoordAddressModeConfig
		{
			ETextureAddressMode::ClampToEdge,
			ETextureAddressMode::ClampToEdge,
			ETextureAddressMode::ClampToEdge,
		},
		math::RGBAColorR32Norm
		{
			1.0f,
			1.0f,
			1.0f,
			1.0f,
		},
		ETextureBorderPredefinedColor::OpaqueWhite,
		TextureFilterConfig
		{
			0,
			ETextureFilter::Linear,
			ETextureFilter::Linear,
			ETextureMipMode::Linear,
		},
		SamplerConfig::MipLODRange
		{
			-1024.0f,
			1024.0f,
		},
		0.0f,
		ETextureCompareMode::None,
		ECompFunc::Never
	};

} // namespace ts3::gpuapi
