
#pragma once

#ifndef __TS3_GPUAPI_COMMON_DEFS_H__
#define __TS3_GPUAPI_COMMON_DEFS_H__

namespace ts3::gpuapi
{

	ts3EnableCustomExceptionSupport();
	ts3EnableEnumTypeInfoSupport();

	using gpu_resource_id_t = uint64;

	template <typename TpClass>
	using GpaHandle = SharedHandle<TpClass>;

	enum EGPUSystemMetrics : uint32
	{
		E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM = 16,
		E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM = 16,
		E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM = 8,
		E_GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM = 9,
		E_GPU_SYSTEM_METRIC_SHADER_COMBINED_STAGES_NUM = 8,
		E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM = 7,
		E_GPU_SYSTEM_METRIC_IS_MAX_CONSTANT_GROUP_SIZE = 32,
		E_GPU_SYSTEM_METRIC_IS_MAX_DESCRIPTOR_SET_SIZE = 16,
		E_GPU_SYSTEM_METRIC_IS_MAX_DESCRIPTOR_SETS_NUM = 4,
		E_GPU_SYSTEM_METRIC_IS_MAX_DWORD_SIZE = 64,
		E_GPU_SYSTEM_METRIC_TEXTURE_MAX_MIP_LEVELS_NUM = 16
	};

	enum EGPUDriverConfigFlags : uint32
	{
		E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT           = 0x0001,
		E_GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT     = 0x0002 | E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT,
		E_GPU_DRIVER_CONFIG_FLAG_DISABLE_MULTI_THREAD_ACCESS_BIT  = 0x0010,
		E_GPU_DRIVER_CONFIG_FLAG_FORCE_COMPATIBILITY_BIT          = 0x0100,
		E_GPU_DRIVER_CONFIG_FLAG_FORCE_CORE_PROFILE_BIT           = 0x0200,
		E_GPU_DRIVER_CONFIG_FLAG_USE_REFERENCE_DRIVER_BIT         = 0x8000,
		GPU_DRIVER_CONFIG_FLAGS_DEFAULT                         = 0
	};

	/// @brief A set of index values for supported shader stages.
	///
	/// @see EPipelineStageIndex
	enum EShaderStageIndex : uint32
	{
		/// Index of the vertex shader stage.
		E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,

		/// Index of the tesselation control shader stage.
		E_SHADER_STAGE_INDEX_GRAPHICS_TESS_CONTROL,

		/// Index of the tesselation evaluation shader stage.
		E_SHADER_STAGE_INDEX_GRAPHICS_TESS_EVALUATION,

		/// Index of the geometry shader stage.
		E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,

		/// Index of the amplification shader stage.
		E_SHADER_STAGE_INDEX_GRAPHICS_AMPLIFICATION,

		/// Index of the mesh shader stage.
		E_SHADER_STAGE_INDEX_GRAPHICS_MESH,

		/// Index of the pixel shader stage.
		E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,

		/// Index of the compute shader stage.
		E_SHADER_STAGE_INDEX_COMPUTE,

		/// Base stage index, i.e. index of the first supported stage. Values below this one are not valid stage indexes.
		/// To compute a zero-based index, subtract this from a valid stage index or use ecGetShaderStageAbsoluteIndex().
		E_SHADER_STAGE_INDEX_BASE = E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,

		/// Index of the last graphics stage. Used to verify if a specified index is a valid graphics stage index.
		E_SHADER_STAGE_INDEX_MAX_GRAPHICS = E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,

		/// Index of the last stage (in the whole pipeline). Values above this one are not valid stage indexes.
		E_SHADER_STAGE_INDEX_MAX = E_SHADER_STAGE_INDEX_COMPUTE,

		/// An explicitly invalid stage index.
		E_SHADER_STAGE_INDEX_INVALID = 0xFFFF
	};

	/// @brief Returns an EShaderStageIndex matching the specified shader stage index value.
	/// @return Corresponding EShaderStageIndex for valid index values or E_SHADER_STAGE_INDEX_INVALID otherwise.
	TS3_FUNC_NO_DISCARD inline constexpr EShaderStageIndex ecGetShaderStageIndexFromValue( uint32 pStageIndex )
	{
		return ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX ) ? static_cast<EShaderStageIndex>( pStageIndex ) : E_SHADER_STAGE_INDEX_INVALID;
	}

	/// @brief Bit flags for all supported shader stages.
	///
	/// Each flag is a value with a single bit set, uniquely identifying the corresponding shader stage.
	/// The position of this bit is the EShaderStageIndex value for the particular stage.
	///
	/// @see EShaderStageIndex
	enum EShaderStageFlags : uint32
	{
		// Note: ecDeclareShaderType() and other enum utility functions assume, that shader-related bits will fit
		// 16-bit value range and have values up to 0xFFFF (no shifting is performed). In case of any alterations,
		// update that part of the functionality accordingly (see shaderCommon.h).

		/// Graphics pipeline: vertex shader bit.
		E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT          = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,

		/// Graphics pipeline: tesselation control shader bit.
		E_SHADER_STAGE_FLAG_GRAPHICS_TESS_CONTROL_BIT    = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_TESS_CONTROL,

		/// Graphics pipeline: tesselation evaluation shader bit.
		E_SHADER_STAGE_FLAG_GRAPHICS_TESS_EVALUATION_BIT = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_TESS_EVALUATION,

		/// Graphics pipeline: geometry shader bit
		E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT        = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,

		/// Graphics pipeline: amplification shader bit.
		E_SHADER_STAGE_FLAG_GRAPHICS_AMPLIFICATION_BIT   = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_AMPLIFICATION,

		/// Graphics pipeline: mesh shader bit
		E_SHADER_STAGE_FLAG_GRAPHICS_MESH_BIT            = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_MESH,

		/// Graphics pipeline: pixel shader bit.
		E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT           = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,

		/// Compute pipeline: compute shader bit.
		E_SHADER_STAGE_FLAG_COMPUTE_BIT                  = 1u << E_SHADER_STAGE_INDEX_COMPUTE,

		/// Mask with all bits for all graphics stages set.
		E_SHADER_STAGE_MASK_GRAPHICS_ALL                 = 0x007F,

		/// Mask with all bits for all supported stages (for both graphics and compute) set.
		E_SHADER_STAGE_MASK_ALL                          = E_SHADER_STAGE_MASK_GRAPHICS_ALL | E_SHADER_STAGE_FLAG_COMPUTE_BIT,

		/// Mask with no valid bits set (integral value is 0).
		E_SHADER_STAGE_MASK_NONE                         = 0u,
	};

	/// @brief Returns a 32-bit value which is a bit flag matching the shader stage specified using its index.
	/// @return One of E_SHADER_STAGE_FLAG_xxx values for a valid stage index or 0 otherwise, returned as uint32.
	TS3_FUNC_NO_DISCARD inline constexpr uint32 ecMakeShaderStageBit( uint32 pStageIndex )
	{
		return ( 1 << pStageIndex ) & E_SHADER_STAGE_MASK_ALL;
	}

	/// @brief
	TS3_FUNC_NO_DISCARD inline constexpr EShaderStageFlags ecMakeShaderStageFlag( uint32 pStageIndex )
	{
		return static_cast<EShaderStageFlags>( ecMakeShaderStageBit( pStageIndex ) );
	}

	/// @brief Returns a 32-bit value which is a bit flag matching the graphics shader stage specified using its index.
	/// @return One of E_SHADER_STAGE_FLAG_GRAPHICS_xxx values for a valid stage index or 0 otherwise, returned as uint32.
	TS3_FUNC_NO_DISCARD inline constexpr uint32 ecMakeGraphicsShaderStageBit( uint32 pGraphicsStageIndex )
	{
		return ( 1 << pGraphicsStageIndex ) & E_SHADER_STAGE_MASK_GRAPHICS_ALL;
	}

	/// @brief
	TS3_FUNC_NO_DISCARD inline constexpr EShaderStageFlags ecMakeGraphicsShaderStageFlag( uint32 pGraphicsStageIndex )
	{
		return static_cast<EShaderStageFlags>( ecMakeGraphicsShaderStageBit( pGraphicsStageIndex ) );
	}

	/// @brief A set of index values for supported pipeline stages.
	///
	/// Just like EShaderStageIndex defines indexes for shader stages, this is a set of values for the whole pipeline
	/// (including shader stages as well). Not all of the defined stages may refer to the actual, hardware-reflected
	/// subset of the pipeline. Some of the values may be "pseudo-stages", indicating execution of some host-side commands.
	///
	/// @note
	/// Values for shader stages (E_PIPELINE_STAGE_INDEX_SHADER_xxx) are guaranteed to be the same as their
	/// EShaderStageIndex counterparts (E_SHADER_STAGE_INDEX_xxx). That of course means that are 0-based too.
	enum EPipelineStageIndex : uint32
	{
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_VERTEX          = E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_TESS_CONTROL    = E_SHADER_STAGE_INDEX_GRAPHICS_TESS_CONTROL,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_TESS_EVALUATION = E_SHADER_STAGE_INDEX_GRAPHICS_TESS_EVALUATION,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_GEOMETRY        = E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_AMPLIFICATION   = E_SHADER_STAGE_INDEX_GRAPHICS_AMPLIFICATION,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_MESH            = E_SHADER_STAGE_INDEX_GRAPHICS_MESH,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_PIXEL           = E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,
		E_PIPELINE_STAGE_INDEX_SHADER_COMPUTE                  = E_SHADER_STAGE_INDEX_COMPUTE,
		E_PIPELINE_STAGE_INDEX_SHADER_MAX                      = E_SHADER_STAGE_INDEX_MAX,

		E_PIPELINE_STAGE_INDEX_GENERIC_HOST_ACCESS,
		E_PIPELINE_STAGE_INDEX_GENERIC_RESOLVE,
		E_PIPELINE_STAGE_INDEX_GENERIC_TRANSFER,
		E_PIPELINE_STAGE_INDEX_IA_INDIRECT_ARGUMENTS_READ,
		E_PIPELINE_STAGE_INDEX_IA_VERTEX_INPUT_DATA_READ,
		E_PIPELINE_STAGE_INDEX_FRAGMENT_TEST_EARLY_DS_OP_LOAD,
		E_PIPELINE_STAGE_INDEX_FRAGMENT_TEST_LATE_DS_OP_STORE,
		E_PIPELINE_STAGE_INDEX_RT_COLOR_OUTPUT,

		E_PIPELINE_STAGE_INDEX_BASE = E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_VERTEX,
		E_PIPELINE_STAGE_INDEX_MAX = E_PIPELINE_STAGE_INDEX_RT_COLOR_OUTPUT
	};

	/// @brief Bit flags for all supported shader stages.
	///
	/// Each flag is a value with a single bit set, uniquely identifying the corresponding shader stage.
	/// The position of this bit is the EShaderStageIndex value for the particular stage.
	///
	/// @see EShaderStageIndex
	enum EPipelineStageFlags : uint32
	{
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_VERTEX_BIT          = E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_TESS_CONTROL_BIT    = E_SHADER_STAGE_FLAG_GRAPHICS_TESS_CONTROL_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_TESS_EVALUATION_BIT = E_SHADER_STAGE_FLAG_GRAPHICS_TESS_EVALUATION_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_GEOMETRY_BIT        = E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_AMPLIFICATION_BIT   = E_SHADER_STAGE_FLAG_GRAPHICS_AMPLIFICATION_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_MESH_BIT            = E_SHADER_STAGE_FLAG_GRAPHICS_MESH_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_PIXEL_BIT           = E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_COMPUTE_BIT                  = E_PIPELINE_STAGE_INDEX_SHADER_COMPUTE,

		E_PIPELINE_STAGE_FLAG_GENERIC_HOST_ACCESS_BIT             = 1 << E_PIPELINE_STAGE_INDEX_GENERIC_HOST_ACCESS,
		E_PIPELINE_STAGE_FLAG_GENERIC_RESOLVE_BIT                 = 1 << E_PIPELINE_STAGE_INDEX_GENERIC_RESOLVE,
		E_PIPELINE_STAGE_FLAG_GENERIC_TRANSFER_BIT                = 1 << E_PIPELINE_STAGE_INDEX_GENERIC_TRANSFER,
		E_PIPELINE_STAGE_FLAG_IA_INDIRECT_ARGUMENTS_READ_BIT      = 1 << E_PIPELINE_STAGE_INDEX_IA_INDIRECT_ARGUMENTS_READ,
		E_PIPELINE_STAGE_FLAG_IA_VERTEX_INPUT_DATA_READ_BIT       = 1 << E_PIPELINE_STAGE_INDEX_IA_VERTEX_INPUT_DATA_READ,
		E_PIPELINE_STAGE_FLAG_FRAGMENT_TEST_EARLY_DS_OP_LOAD_BIT  = 1 << E_PIPELINE_STAGE_INDEX_FRAGMENT_TEST_EARLY_DS_OP_LOAD,
		E_PIPELINE_STAGE_FLAG_FRAGMENT_TEST_LATE_DS_OP_STORE_BIT  = 1 << E_PIPELINE_STAGE_INDEX_FRAGMENT_TEST_LATE_DS_OP_STORE,
		E_PIPELINE_STAGE_FLAG_RT_COLOR_OUTPUT_BIT                 = 1 << E_PIPELINE_STAGE_INDEX_RT_COLOR_OUTPUT,
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_DEFS_H__
