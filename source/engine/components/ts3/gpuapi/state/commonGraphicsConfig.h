
#pragma once

#ifndef __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_H__
#define __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	enum EBlendConfigFlags : uint16
	{
		E_BLEND_CONFIG_FLAG_ENABLE_ALPHA_TO_COVERAGE_BIT = 0x01,
		E_BLEND_CONFIG_FLAG_ENABLE_MULTI_RENDER_TARGET_BIT = 0x02,
		E_BLEND_CONFIG_FLAG_ENABLE_MRT_INDEPENDENT_BLENDING_BIT = 0x04,
		E_BLEND_CONFIG_FLAGS_DEFAULT = 0,
		E_BLEND_CONFIG_MASK_ALL = 0x07,
	};

	enum EBlendWriteMaskFlags : uint16
	{
		E_BLEND_WRITE_MASK_NONE = 0,
		E_BLEND_WRITE_MASK_CHANNEL_RED = 0x1,
		E_BLEND_WRITE_MASK_CHANNEL_GREEN = 0x2,
		E_BLEND_WRITE_MASK_CHANNEL_BLUE = 0x4,
		E_BLEND_WRITE_MASK_CHANNEL_ALPHA = 0x8,
		E_BLEND_WRITE_MASK_ALL = 0xF,
	};

	enum EDepthStencilConfigFlags : uint16
	{
		E_DEPTH_STENCIL_CONFIG_FLAG_ENABLE_DEPTH_TEST_BIT = 0x01,
		E_DEPTH_STENCIL_CONFIG_FLAG_ENABLE_STENCIL_TEST_BIT = 0x02,
		E_DEPTH_STENCIL_CONFIG_MASK_ALL = 0x03,
	};

	enum ERasterizerConfigFlags : uint16
	{
		E_RASTERIZER_CONFIG_FLAG_ENABLE_SCISSOR_TEST_BIT = 0x01,
		E_RASTERIZER_CONFIG_MASK_ALL = 0x01
	};

	enum class EBlendFactor : uint16
	{
		Zero,
		One,
		Const,
		ConstInv,
		SrcColor,
		SrcAlpha,
		DstColor,
		DstAlpha,
		SrcColorInv,
		SrcAlphaInv,
		DstColorInv,
		DstAlphaInv
	};

	enum class EBlendOp : uint16
	{
		Add,
		Min,
		Max,
		Subtract,
		SubtractRev
	};

	enum class ECompFunc : uint16
	{
		Never,
		Always,
		Equal,
		NotEqual,
		Greater,
		GreaterEqual,
		Less,
		LessEqual
	};

	enum class ECullMode : uint16
	{
		None,
		Back,
		Front
	};

	enum class EDepthWriteMask : uint16
	{
		All = Limits<uint16>::maxValue,
		None = 0
	};

	enum class EStencilOp : uint16
	{
		Zero,
		Keep,
		Replace,
		IncrClamp,
		IncrWrap,
		DecrClamp,
		DecrWrap,
		Invert
	};

	/// @brief Blend configuration for a single render target attachment (texture/render buffer).
	/// @see BlendConfig
	struct RTColorAttachmentBlendSettings
	{
		math::RGBAColorU8 constantFactor;
		EBlendFactor factorSrcColor;
		EBlendFactor factorSrcAlpha;
		EBlendFactor factorDstColor;
		EBlendFactor factorDstAlpha;
		EBlendOp opColor;
		EBlendOp opAlpha;
		Bitmask<EBlendWriteMaskFlags> renderTargetWriteMask;
	};

	/// @brief A configuration of the depth test for the depth-stencil stage.
	/// Together with the StencilTestSettings struct, it forms DepthStencilConfig used for depth-stencil configuration.
	/// @see StencilTestSettings
	/// @see DepthStencilConfig
	struct DepthTestSettings
	{
		ECompFunc depthCompFunc;
		EDepthWriteMask depthWriteMask;
	};

	/// @brief A configuration of the stencil test for the depth-stencil stage.
	/// Together with the DepthTestSettings struct, it forms DepthStencilConfig used for depth-stencil configuration.
	/// @see DepthTestSettings
	/// @see DepthStencilConfig
	struct StencilTestSettings
	{
		struct FaceOp
		{
			ECompFunc compFunc;
			EStencilOp opFail;
			EStencilOp opPassDepthFail;
			EStencilOp opPassDepthPass;
		};

		FaceOp frontFace;
		FaceOp backFace;
		uint8 refValue;
		uint8 readMask;
		uint8 writeMask;
	};

	/// @brief Represents blend configuration for the graphics pipeline. Used to create BlendStateDescriptor.
	struct BlendConfig
	{
		/// A bitmask with RT attachments for which the blending is enabled.
		/// Used to validate the blend configuration against the RT layout.
		Bitmask<ERTAttachmentFlags> attachmentsMask = 0;

		/// Blending flags.
		Bitmask<EBlendConfigFlags> flags = 0;

		/// An array of blend settings for each RT attachment.
		/// If ENABLE_MRT_SEPARATE_CONFIG_BIT is set, each active target uses its corresponding entry.
		/// Otherwise, attachments[0] is used for all targets and rest of the array is ignored.
		/// @see EBlendConfigFlags
		RTColorAttachmentBlendSettings attachments[cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
	};

	/// @brief
	struct DepthStencilConfig
	{
		Bitmask<EDepthStencilConfigFlags> commonFlags = 0;
		DepthTestSettings depthTestSettings;
		StencilTestSettings stencilTestSettings;
	};

	/// @brief
	struct RasterizerConfig
	{
		Bitmask<ERasterizerConfigFlags> flags = 0;
		ECullMode cullMode;
		EPrimitiveFillMode primitiveFillMode;
		ETriangleVerticesOrder frontFaceVerticesOrder;
	};

	namespace defaults
	{

		/// @brief A default blend configuration for an RT attachment.
		/// This default config represents default set of options used for blending. The config is as follows:
		/// - constantFactor:        {0,0,0,0}
		/// - factorSrcColor:        EBlendFactor::One
		/// - factorSrcAlpha:        EBlendFactor::One
		/// - factorDstColor:        EBlendFactor::Zero
		/// - factorDstAlpha:        EBlendFactor::Zero
		/// - opColor:               EBlendOp::Add
		/// - opAlpha:               EBlendOp::Add
		/// - renderTargetWriteMask: E_BLEND_WRITE_MASK_ALL
		/// @see RTAttachmentBlendConfig
		TS3_GPUAPI_OBJ const RTColorAttachmentBlendSettings cvCommonRTColorAttachmentBlendSettingsDefault;

		/// @brief A default configuration for the depth test.
		/// This default config represents default set of options used for depth testing. The config is as follows:
		/// - depthCompFunc:  ECompFunc::Less
		/// - depthWriteMask: EDepthWriteMask::All
		TS3_GPUAPI_OBJ const DepthTestSettings cvCommonDepthTestSettingsDefault;

		/// @brief A default configuration for the stencil test.
		/// This default config represents default set of options used for stencil testing. The config is as follows:
		/// - frontFace.compFunc:        ECompFunc::Always
		/// - frontFace.opFail:          EStencilOp::Keep
		/// - frontFace.opPassDepthFail: EStencilOp::Keep
		/// - frontFace.opPassDepthPass: EStencilOp::Keep
		/// - backFace.compFunc:         ECompFunc::Always
		/// - backFace.opFail:           EStencilOp::Keep
		/// - backFace.opPassDepthFail:  EStencilOp::Keep
		/// - backFace.opPassDepthPass:  EStencilOp::Keep
		/// - refValue:                  0
		/// - readMask:                  E_STENCIL_MASK_READ_DEFAULT
		/// - writeMask:                 E_STENCIL_MASK_WRITE_DEFAULT
		TS3_GPUAPI_OBJ const StencilTestSettings cvCommonStencilTestSettingsDefault;

		///
		TS3_GPUAPI_OBJ const BlendConfig cvPipelineBlendConfigDefault;

		///
		TS3_GPUAPI_OBJ const DepthStencilConfig cvPipelineDepthStencilConfigDefault;

		///
		TS3_GPUAPI_OBJ const RasterizerConfig cvPipelineRasterizerConfigDefault;

	}

	namespace smutil
	{

		TS3_GPUAPI_API_NO_DISCARD Bitmask<ERTAttachmentFlags> getBlendActiveAttachmentMask( const BlendConfig & pBlendConfig );

		TS3_GPUAPI_API_NO_DISCARD uint32 getBlendActiveAttachmentsNum( const BlendConfig & pBlendConfig );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_GRAPHICS_CONFIG_H__
