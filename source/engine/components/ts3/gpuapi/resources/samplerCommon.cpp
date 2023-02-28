
#include "samplerCommon.h"

namespace ts3::gpuapi
{

	const SamplerDesc cvSamplerDescDefault =
	{
		TextureCoordAddressModeConfig
		{
			ETextureAddressMode::Clamp,
			ETextureAddressMode::Clamp,
			ETextureAddressMode::Clamp,
		},
		math::RGBAColorR32Norm
		{
			1.0f,
			1.0f,
			1.0f,
			1.0f,
		},
		ETextureFilterConfig
		{
			0,
			ETextureFilter::Linear,
			ETextureFilter::Linear,
			ETextureMipMode::Linear,
		},
		SamplerDesc::MipLODRange
		{
			-1024.0f,
			1024.0f,
		},
		0.0f,
		ETextureCompareState::Disabled,
		ECompFunc::Never
	};

} // namespace ts3::gpuapi
