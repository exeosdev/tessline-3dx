
#pragma once

#ifndef __TS3_GPUAPI_COMMON_STATE_DEFS_H__
#define __TS3_GPUAPI_COMMON_STATE_DEFS_H__

#include "../memory/commonGPUMemoryDefs.h"
#include "../resources/commonGPUResourceDefs.h"
#include <ts3/stdext/hash.h>

namespace ts3::gpuapi
{

	struct BlendConfig;
	struct DepthStencilConfig;
	struct GraphicsShaderSet;
	struct IAInputLayoutDefinition;
	struct IAVertexStreamDefinition;
	struct RasterizerConfig;
	struct RenderTargetBindingDefinition;
	struct RenderPassConfiguration;

	struct GraphicsPipelineStateObjectCreateInfo;

	ts3GpaDeclareClassHandle( GPUDeviceChildObject );
	ts3GpaDeclareClassHandle( PipelineStateObject );
	ts3GpaDeclareClassHandle( ComputePipelineStateObject );
	ts3GpaDeclareClassHandle( GraphicsPipelineStateObject );

	ts3GpaDeclareClassHandle( BlendImmutableState );
	ts3GpaDeclareClassHandle( DepthStencilImmutableState );
	ts3GpaDeclareClassHandle( GraphicsShaderLinkageImmutableState );
	ts3GpaDeclareClassHandle( IAInputLayoutImmutableState );
	ts3GpaDeclareClassHandle( IAVertexStreamImmutableState );
	ts3GpaDeclareClassHandle( RasterizerImmutableState );
	ts3GpaDeclareClassHandle( RenderTargetBindingImmutableState );
	ts3GpaDeclareClassHandle( RenderPassImmutableState );

	using pipeline_internal_state_hash_t = uint64;
	using render_target_index_t = uint16;
	using shader_input_ref_id_t = uint64;
	using shader_input_index_t = uint32;

	using GraphicsShaderArray = std::array<ShaderHandle, cxdefs::GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM>;

	namespace cxdefs
	{

		///
		constexpr auto PIPELINE_INTERNAL_STATE_ID_INVALID = 0u;

		///
		constexpr auto VERTEX_ATTRIBUTE_OFFSET_APPEND = Limits<gpu_memory_size_t>::maxValue;

		///
		constexpr auto RT_MAX_COLOR_ATTACHMENTS_NUM = static_cast<render_target_index_t>( cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM );

		///
		constexpr auto RT_MAX_COMBINED_ATTACHMENTS_NUM = static_cast<render_target_index_t>( cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM );

		///
		constexpr auto RT_ATTACHMENT_MSAA_LEVEL_INVALID = Limits<uint32>::maxValue;

		///
		constexpr auto PIPELINE_IMMUTABLE_STATE_TYPES_NUM = 8u;

		/// @brief
		inline constexpr uint32 makeRTAttachmentFlag( render_target_index_t pAttachmentIndex )
		{
			return ( pAttachmentIndex < RT_MAX_COMBINED_ATTACHMENTS_NUM ) ? ( 1 << pAttachmentIndex ) : 0u;
		}

		/// @brief
		inline constexpr bool isRTAttachmentIndexValid( render_target_index_t pIndex )
		{
			return pIndex < cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM;
		}

	}

	enum EPipelineImmutableStateTypeFlags : uint32
	{
		E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_BLEND_BIT = 0x0001,
		E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_DEPTH_STENCIL_BIT = 0x0002,
		E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_GRAPHICS_SHADER_LINKAGE_BIT = 0x0008,
		E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_IA_INPUT_LAYOUT_BIT = 0x0010,
		E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_IA_VERTEX_STREAM_BIT = 0x0020,
		E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_RASTERIZER_BIT = 0x0004,
		E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_RENDER_TARGET_BINDING_BIT = 0x0040,
		E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_RENDER_PASS_BIT = 0x0080,
		E_PIPELINE_IMMUTABLE_STATE_TYPE_MASK_ALL = 0x00FF,
	};

	/// @brief
	enum ERTAttachmentIndex : render_target_index_t
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
	enum ERTAttachmentFlags : uint32
	{
		E_RT_ATTACHMENT_FLAG_COLOR_0_BIT       = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_COLOR_0 ),
		E_RT_ATTACHMENT_FLAG_COLOR_1_BIT       = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_COLOR_1 ),
		E_RT_ATTACHMENT_FLAG_COLOR_2_BIT       = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_COLOR_2 ),
		E_RT_ATTACHMENT_FLAG_COLOR_3_BIT       = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_COLOR_3 ),
		E_RT_ATTACHMENT_FLAG_COLOR_4_BIT       = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_COLOR_4 ),
		E_RT_ATTACHMENT_FLAG_COLOR_5_BIT       = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_COLOR_5 ),
		E_RT_ATTACHMENT_FLAG_COLOR_6_BIT       = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_COLOR_6 ),
		E_RT_ATTACHMENT_FLAG_COLOR_7_BIT       = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_COLOR_7 ),
		E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT = cxdefs::makeRTAttachmentFlag( E_RT_ATTACHMENT_INDEX_DEPTH_STENCIL ),

		E_RT_ATTACHMENT_MASK_COLOR_ALL = makeLSFBitmask<uint32>( cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM ),
		E_RT_ATTACHMENT_MASK_ALL = E_RT_ATTACHMENT_MASK_COLOR_ALL | E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT,
		E_RT_ATTACHMENT_FLAGS_DEFAULT_C0_DS = E_RT_ATTACHMENT_FLAG_COLOR_0_BIT | E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT,
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

	struct RenderTargetAttachmentClearConfig
	{
		math::RGBAColorR32Norm colorValue;
		float depthValue;
		uint8 stencilValue;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_STATE_DEFS_H__
