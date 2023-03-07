
#include "commonGraphicsConfig.h"

namespace ts3::gpuapi
{

	namespace defaults
	{

		const RTColorAttachmentBlendSettings cvCommonRTColorAttachmentBlendSettingsDefault =
		{
			{ 0u, 0u, 0u, 0u },
			EBlendFactor::One,
			EBlendFactor::One,
			EBlendFactor::Zero,
			EBlendFactor::Zero,
			EBlendOp::Add,
			EBlendOp::Add,
			E_BLEND_WRITE_MASK_ALL
		};

		const DepthTestSettings cvCommonDepthTestSettingsDefault =
		{
			ECompFunc::Less,
			EDepthWriteMask::All
		};

		const StencilTestSettings cvCommonStencilTestSettingsDefault =
		{
			{
				ECompFunc::Always,
				EStencilOp::Keep,
				EStencilOp::Keep,
				EStencilOp::Keep
			},
			{
				ECompFunc::Always,
				EStencilOp::Keep,
				EStencilOp::Keep,
				EStencilOp::Keep
			},
			0,
			0xFF,
			0xFF
		};

		const BlendConfig cvPipelineBlendConfigDefault =
		{
			E_RT_ATTACHMENT_FLAG_COLOR_0_BIT,
			E_BLEND_CONFIG_FLAGS_DEFAULT,
			{
				cvCommonRTColorAttachmentBlendSettingsDefault
			}
		};

		const DepthStencilConfig cvPipelineDepthStencilConfigDefault =
		{
			0u,
			cvCommonDepthTestSettingsDefault,
			cvCommonStencilTestSettingsDefault
		};

		const RasterizerConfig cvPipelineRasterizerConfigDefault =
		{
			0u,
			ECullMode::Back,
			EPrimitiveFillMode::Solid,
			ETriangleVerticesOrder::Clockwise,
		};

	}

} // namespace ts3::gpuapi
