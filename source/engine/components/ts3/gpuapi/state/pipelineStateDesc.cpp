
#include "pipelineStateDesc.h"

namespace ts3::gpuapi
{

	static const DepthStencilConfigDesc::DepthSettings sDepthSettingsDepthDisabled =
	{
		ECompFunc::Always,
		EDepthWriteMask::All
	};

	static const DepthStencilConfigDesc::StencilSettings sStencilSettingsStencilDisabled =
	{
		DepthStencilConfigDesc::StencilFaceOp
		{
			ECompFunc::Always,
			EStencilOp::Keep,
			EStencilOp::Keep,
			EStencilOp::Keep
		},
		DepthStencilConfigDesc::StencilFaceOp
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

	const BlendConfigDesc cvPipelineDescBlendConfigDisabled =
	{
		EBlendState::Disabled,
		math::RGBAColorU8{ 0xFF, 0xFF, 0xFF, 0xFF },
		EBlendFactor::SrcColor,
		EBlendFactor::SrcAlpha,
		EBlendFactor::DstColor,
		EBlendFactor::DstAlpha,
		EBlendOp::Add,
		EBlendOp::Add,
		EBlendRenderTargetWriteMask::All
	};

	const BlendConfigDesc cvPipelineDescBlendConfigEnabledAdd =
	{
			EBlendState::Enabled,
		math::RGBAColorU8{ 0u, 0u, 0u, 0u },
		EBlendFactor::SrcColor,
		EBlendFactor::SrcAlpha,
		EBlendFactor::DstColor,
		EBlendFactor::DstAlpha,
		EBlendOp::Add,
		EBlendOp::Add
	};

	const DepthStencilConfigDesc cvPipelineDescDepthStencilConfigDisabled =
	{
		EDepthTestState::Disabled,
		sDepthSettingsDepthDisabled,
		EStencilTestState::Disabled,
		sStencilSettingsStencilDisabled
	};

	const DepthStencilConfigDesc cvPipelineDescDepthStencilConfigEnabledDepthOnly =
	{
		EDepthTestState::Enabled,
		{
			ECompFunc::Less,
			EDepthWriteMask::All
		},
		EStencilTestState::Disabled,
		sStencilSettingsStencilDisabled
	};

	const RasterizerConfigDesc cvPipelineDescRasterizerConfigDefault =
	{
		ECullMode::Back,
		EPrimitiveFillMode::Solid,
		ETriangleVerticesOrder::CounterClockwise,
		EScissorTestState::Disabled
	};

	const VertexInputFormatDesc cvPipelineDescVertexInputFormatEmpty =
	{
		VertexInputFormatDesc::VertexAttributeArray
		{
		},
		EPrimitiveTopology::TriangleList
	};

	const VertexInputFormatDesc cvPipelineDescVertexInputFormatVp3 =
	{
		VertexInputFormatDesc::VertexAttributeArray
		{
			VertexInputFormatDesc::VertexAttribute
			{
				0,
				0,
				"POSITION",
				0,
				EVertexAttribFormat::VEC3_FLOAT32,
				0,
				0
			}
		},
		EPrimitiveTopology::TriangleList
	};

	const VertexInputFormatDesc cvPipelineDescVertexInputFormatVp3Vc4 =
	{
		VertexInputFormatDesc::VertexAttributeArray
		{
			VertexInputFormatDesc::VertexAttribute
			{
				0,
				0,
				"POSITION",
				0,
				EVertexAttribFormat::VEC3_FLOAT32,
				0,
				0
			},
			VertexInputFormatDesc::VertexAttribute
			{
				1,
				0,
				"COLOR",
				0,
				EVertexAttribFormat::VEC4_UBYTE_NORM,
				CX_VERTEX_ATTRIBUTE_OFFSET_APPEND,
				0
			}
		},
		EPrimitiveTopology::TriangleList
	};

} // namespace ts3::gpuapi
