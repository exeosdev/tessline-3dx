
#pragma once

#ifndef __TS3_GPUAPI_COMMON_DEFS_H__
#define __TS3_GPUAPI_COMMON_DEFS_H__

namespace ts3::gpuapi
{

	ts3EnableCustomExceptionSupport();
	ts3EnableEnumTypeInfoSupport();

	template <typename TClass>
	using GpaHandle = SharedHandle<TClass>;

	using UniqueGPUObjectID = HFSIdentifier;
	using UniqueGPUObjectName = std::string;

	template <typename TStrInput>
	inline UniqueGPUObjectID generateUniqueGPUObjectID( TStrInput && pStrInput )
	{
		return generateHFSIdentifier( pStrInput );
	}

#define ts3GpaDeclareClassHandle( pClassName ) \
    class pClassName; \
    using pClassName##Handle = SharedHandle<pClassName>; \
    using pClassName##WeakHandle = WeakHandle<pClassName>

	namespace cxdefs
	{

		constexpr uint32 GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM = 16;
		constexpr uint32 GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM = 16;
		constexpr uint32 GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM = 8;
		constexpr uint32 GPU_SYSTEM_METRIC_RT_MAX_COMBINED_ATTACHMENTS_NUM = 9;
		constexpr uint32 GPU_SYSTEM_METRIC_SHADER_COMBINED_STAGES_NUM = 6;
		constexpr uint32 GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM = 5;
		constexpr uint32 GPU_SYSTEM_METRIC_IS_MAX_CONSTANT_GROUP_SIZE = 32;
		constexpr uint32 GPU_SYSTEM_METRIC_IS_MAX_DESCRIPTOR_SET_SIZE = 16;
		constexpr uint32 GPU_SYSTEM_METRIC_IS_MAX_DESCRIPTOR_SETS_NUM = 4;
		constexpr uint32 GPU_SYSTEM_METRIC_IS_MAX_DWORD_SIZE = 64;
		constexpr uint32 GPU_SYSTEM_METRIC_TEXTURE_MAX_MIP_LEVELS_NUM = 16;

		/// A special constant which can be used for object IDs to indicate that ID should be assigned automatically.
		/// In most cases it is safe to assume that object address will be used as the ID (unless stated otherwise).
		inline constexpr UniqueGPUObjectID GPU_OBJECT_ID_AUTO { Limits<uint64>::maxValue };

		/// An invalid object ID. Such IDs may refer to objects which are either uninitialised, marked for deletion,
		/// or do not yet exist in the object management system. This ID also means "not found" in case of queries.
		inline constexpr UniqueGPUObjectID GPU_OBJECT_ID_INVALID { Limits<uint64>::maxValue - 1 };

		///
		inline constexpr UniqueGPUObjectID GPU_OBJECT_ID_EMPTY { 0 };

		inline constexpr bool isUniqueGPUObjectIDValid( UniqueGPUObjectID pUniqueID ) noexcept
		{
			return ( pUniqueID != GPU_OBJECT_ID_INVALID ) && ( pUniqueID != GPU_OBJECT_ID_EMPTY );
		}

	}

	enum EGPUDriverConfigFlags : uint32
	{
		E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT           = 0x0001,
		E_GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT     = 0x0002 | E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT,
		E_GPU_DRIVER_CONFIG_FLAG_DISABLE_MULTI_THREAD_ACCESS_BIT  = 0x0010,
		E_GPU_DRIVER_CONFIG_FLAG_FORCE_COMPATIBILITY_BIT          = 0x0100,
		E_GPU_DRIVER_CONFIG_FLAG_FORCE_CORE_PROFILE_BIT           = 0x0200,
		E_GPU_DRIVER_CONFIG_FLAG_USE_REFERENCE_DRIVER_BIT         = 0x8000,
		GPU_DRIVER_CONFIG_FLAGS_DEFAULT                           = 0
	};

	/// @brief A set of index values for supported shader stages.
	///
	/// @see EPipelineStageIndex
	enum EShaderStageIndex : uint32
	{
		E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,
		E_SHADER_STAGE_INDEX_GRAPHICS_HULL,
		E_SHADER_STAGE_INDEX_GRAPHICS_DOMAIN,
		E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,
		E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,
		E_SHADER_STAGE_INDEX_COMPUTE,

		/// Base stage index, i.e. index of the first supported stage. Values below this one are not valid stage indexes.
		/// To compute a zero-based index, subtract this from a valid stage index or use cxdefs::getShaderStageAbsoluteIndex().
		E_SHADER_STAGE_INDEX_BASE = E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,

		/// Index of the last graphics stage. Used to verify if a specified index is a valid graphics stage index.
		E_SHADER_STAGE_INDEX_MAX_GRAPHICS = E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,

		/// Index of the last stage (in the whole pipeline). Values above this one are not valid stage indexes.
		E_SHADER_STAGE_INDEX_MAX = E_SHADER_STAGE_INDEX_COMPUTE,

		/// An explicitly invalid stage index.
		E_SHADER_STAGE_INDEX_INVALID = 0xFFFF
	};

	/// @brief Bit flags for all supported shader stages.
	///
	/// Each flag is a value with a single bit set, uniquely identifying the corresponding shader stage.
	/// The position of this bit is the EShaderStageIndex value for the particular stage.
	///
	/// @see EShaderStageIndex
	enum EShaderStageFlags : uint32
	{
		// Note: cxdefs::declareShaderType() and other enum utility functions assume, that shader-related bits will fit
		// 16-bit value range and have values up to 0xFFFF (no shifting is performed). In case of any alterations,
		// update that part of the functionality accordingly (see shaderCommon.h).

		E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT   = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,
		E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT     = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_HULL,
		E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT   = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_DOMAIN,
		E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,
		E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT    = 1u << E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,
		E_SHADER_STAGE_FLAG_COMPUTE_BIT           = 1u << E_SHADER_STAGE_INDEX_COMPUTE,

		/// Mask with all graphics stages bits set.
		E_SHADER_STAGE_MASK_GRAPHICS_ALL = 0x007F,

		/// Mask with required stages for vertex-based shader setup (vertex shader and pixel shader).
		E_SHADER_STAGE_MASK_GRAPHICS_REQUIRED_VERT = E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT | E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT,

		/// Mask with all bits for all supported stages (for both graphics and compute) set.
		E_SHADER_STAGE_MASK_ALL = E_SHADER_STAGE_MASK_GRAPHICS_ALL | E_SHADER_STAGE_FLAG_COMPUTE_BIT,

		/// Mask with no valid bits set (integral value is 0).
		E_SHADER_STAGE_MASK_NONE = 0u,
	};

	static_assert( E_SHADER_STAGE_MASK_ALL < 0xFFFF );

	/// Constant Expressions
	namespace cxdefs
	{

		/// @brief Returns an EShaderStageIndex matching the specified shader stage index value.
		/// @return Corresponding EShaderStageIndex for valid index values or E_SHADER_STAGE_INDEX_INVALID otherwise.
		TS3_ATTR_NO_DISCARD inline constexpr EShaderStageIndex getShaderStageIndexFromValue( uint32 pStageIndex )
		{
			return ( pStageIndex <= E_SHADER_STAGE_INDEX_MAX ) ? static_cast<EShaderStageIndex>( pStageIndex ) : E_SHADER_STAGE_INDEX_INVALID;
		}

		/// @brief Returns a 32-bit value which is a bit flag matching the shader stage specified using its index.
		/// @return One of E_SHADER_STAGE_FLAG_xxx values for a valid stage index or 0 otherwise, returned as uint32.
		TS3_ATTR_NO_DISCARD inline constexpr uint32 makeShaderStageBit( uint32 pStageIndex )
		{
			return ( 1 << pStageIndex ) & E_SHADER_STAGE_MASK_ALL;
		}

		/// @brief Returns a 32-bit value which is a bit flag matching the graphics shader stage specified using its index.
		/// @return One of E_SHADER_STAGE_FLAG_GRAPHICS_xxx values for a valid stage index or 0 otherwise, returned as uint32.
		TS3_ATTR_NO_DISCARD inline constexpr uint32 makeGraphicsShaderStageBit( uint32 pGraphicsStageIndex )
		{
			return ( 1 << pGraphicsStageIndex ) & E_SHADER_STAGE_MASK_GRAPHICS_ALL;
		}

		/// @brief
		TS3_ATTR_NO_DISCARD inline constexpr EShaderStageFlags makeGraphicsShaderStageFlag( uint32 pGraphicsStageIndex )
		{
			return static_cast<EShaderStageFlags>( makeGraphicsShaderStageBit( pGraphicsStageIndex ) );
		}

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
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_VERTEX   = E_SHADER_STAGE_INDEX_GRAPHICS_VERTEX,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_HULL     = E_SHADER_STAGE_INDEX_GRAPHICS_HULL,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_DOMAIN   = E_SHADER_STAGE_INDEX_GRAPHICS_DOMAIN,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_GEOMETRY = E_SHADER_STAGE_INDEX_GRAPHICS_GEOMETRY,
		E_PIPELINE_STAGE_INDEX_SHADER_GRAPHICS_PIXEL    = E_SHADER_STAGE_INDEX_GRAPHICS_PIXEL,
		E_PIPELINE_STAGE_INDEX_SHADER_COMPUTE           = E_SHADER_STAGE_INDEX_COMPUTE,
		E_PIPELINE_STAGE_INDEX_SHADER_MAX               = E_SHADER_STAGE_INDEX_MAX,

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
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_HULL_BIT            = E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_DOMAIN_BIT          = E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT,
		E_PIPELINE_STAGE_FLAG_SHADER_GRAPHICS_GEOMETRY_BIT        = E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT,
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
