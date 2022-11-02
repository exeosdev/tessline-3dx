
#pragma once

#ifndef __TS3_GPUAPI_COMMON_STATE_DEFS_H__
#define __TS3_GPUAPI_COMMON_STATE_DEFS_H__

#include "../memory/commonGPUMemoryDefs.h"
#include <ts3/stdext/hash.h>

namespace ts3::GpuAPI
{

	ts3GpaDeclareClassHandle( GPUStateObject );
	ts3GpaDeclareClassHandle( PipelineStateObject );
	ts3GpaDeclareClassHandle( GraphicsPipelineStateObject );

	struct GraphicsPipelineStateObjectCreateInfo;

	using pipeline_internal_state_hash_t = uint64;
	using render_target_index_t = uint16;
	using shader_input_ref_id_t = uint64;
	using shader_input_index_t = uint32;

	namespace CxDefs
	{

		constexpr auto PIPELINE_INTERNAL_STATE_ID_INVALID = 0;

		constexpr auto VERTEX_ATTRIBUTE_OFFSET_APPEND = Limits<gpu_memory_size_t>::maxValue;

	}

	/// @brief
	enum ERenderTargetAttachmentIndex : render_target_index_t
	{
		E_RT_ATTACHMENT_INDEX_COLOR_0,
		E_RT_ATTACHMENT_INDEX_COLOR_1,
		E_RT_ATTACHMENT_INDEX_COLOR_2,
		E_RT_ATTACHMENT_INDEX_COLOR_3,
		E_RT_ATTACHMENT_INDEX_COLOR_4,
		E_RT_ATTACHMENT_INDEX_COLOR_5,
		E_RT_ATTACHMENT_INDEX_COLOR_6,
		E_RT_ATTACHMENT_INDEX_COLOR_7,
		E_RT_ATTACHMENT_INDEX_DEPTH_STENCIL
	};

	/// @brief A set of bit flags representing render target attachments.
	enum ERenderTargetAttachmentFlags : uint32
	{
		E_RT_ATTACHMENT_FLAG_COLOR_0_BIT = 1 << E_RT_ATTACHMENT_INDEX_COLOR_0,
		E_RT_ATTACHMENT_FLAG_COLOR_1_BIT = 1 << E_RT_ATTACHMENT_INDEX_COLOR_1,
		E_RT_ATTACHMENT_FLAG_COLOR_2_BIT = 1 << E_RT_ATTACHMENT_INDEX_COLOR_2,
		E_RT_ATTACHMENT_FLAG_COLOR_3_BIT = 1 << E_RT_ATTACHMENT_INDEX_COLOR_3,
		E_RT_ATTACHMENT_FLAG_COLOR_4_BIT = 1 << E_RT_ATTACHMENT_INDEX_COLOR_4,
		E_RT_ATTACHMENT_FLAG_COLOR_5_BIT = 1 << E_RT_ATTACHMENT_INDEX_COLOR_5,
		E_RT_ATTACHMENT_FLAG_COLOR_6_BIT = 1 << E_RT_ATTACHMENT_INDEX_COLOR_6,
		E_RT_ATTACHMENT_FLAG_COLOR_7_BIT = 1 << E_RT_ATTACHMENT_INDEX_COLOR_7,
		E_RT_ATTACHMENT_FLAG_DEPTH_BIT   = E_RT_ATTACHMENT_FLAG_COLOR_7_BIT << 1,
		E_RT_ATTACHMENT_FLAG_STENCIL_BIT = E_RT_ATTACHMENT_FLAG_DEPTH_BIT << 1,

		E_RT_ATTACHMENT_MASK_COLOR_ALL = 0x7F,
		E_RT_ATTACHMENT_MASK_DEPTH_STENCIL = E_RT_ATTACHMENT_FLAG_DEPTH_BIT | E_RT_ATTACHMENT_FLAG_STENCIL_BIT,
		E_RT_ATTACHMENT_MASK_ALL = E_RT_ATTACHMENT_MASK_COLOR_ALL | E_RT_ATTACHMENT_MASK_DEPTH_STENCIL,
		E_RT_ATTACHMENT_FLAGS_DEFAULT_C0_DS = E_RT_ATTACHMENT_FLAG_COLOR_0_BIT | E_RT_ATTACHMENT_MASK_DEPTH_STENCIL,
	};

	enum class EPrimitiveFillMode : uint16
	{
		Solid,
		Wireframe
	};

	enum class EPrimitiveTopology : uint16
	{
		Undefined,
		PointList,
		LineList,
		LineListAdj,
		LineStrip,
		LineStripAdj,
		TriangleList,
		TriangleListAdj,
		TriangleStrip,
		TriangleStripAdj,
		TesselationPatch,
	};

	enum class ETriangleVerticesOrder : uint16
	{
		Clockwise,
		CounterClockwise
	};

	enum class EGraphicsPipelineInternalStateType : uint16
	{
		/// A descriptor of this type represents configuration of blending.
		BlendState,

		/// A descriptor of this type represents configuration of the depth/stencil.
		DepthStencilState,

		/// A descriptor of this type represents configuration of the rasterizer stage.
		RasterizerState,

		///
		RenderTargetLayout,

		///
		ShaderLinkage,

		/// A descriptor of this type represents a vertex input layout configuration.
		VertexInputLayout,

		///
		Unknown
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_COMMON_STATE_DEFS_H__
