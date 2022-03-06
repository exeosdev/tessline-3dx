
#pragma once

#ifndef __TS3_GPUAPI_COMMON_RESOURCE_DEFS_H__
#define __TS3_GPUAPI_COMMON_RESOURCE_DEFS_H__

#include "../memory/commonGPUMemoryDefs.h"

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( GPUResource );
	ts3DeclareClassHandle( GPUBuffer );
	ts3DeclareClassHandle( Sampler );
	ts3DeclareClassHandle( Shader );
	ts3DeclareClassHandle( Texture );

	struct GPUBufferCreateInfo;
	struct SamplerCreateInfo;
	struct ShaderCreateInfo;
	struct TextureCreateInfo;

	struct GPUBufferDataCopyDesc;
	struct GPUBufferSubDataCopyDesc;
	struct GPUBufferDataUploadDesc;
	struct GPUBufferSubDataUploadDesc;

	using resource_flags_value_t = uint32;
	using gpu_resource_size_t = uint32;

	enum class EGPUResourceBaseType : enum_default_value_t
	{
		Buffer,
		Texture,
		Unknown
	};

	/// @brief
	enum EGPUResourceContentFlags : resource_flags_value_t
	{
		// Specifies dynamic content, i.e. content expected to be updated frequently, possibly multiple
		// times per single frame. This flag should be combined with CPU_WRITE access to enable resource
		// mapping and effective write modes like Append or NoOverwrite. Cannot be combined with neither
		// IMMUTABLE nor STATIC usages.
		E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT    = 0x01,

		// Specifies immutable content which must me specified at the creation time and cannot be altered
		// afterwards. No CPU access is allowed for resources created with this flag. Cannot be combined
		// with DYNAMIC usage. Although IMMUTABLE|STATIC is a valid combination, this effectively causes
		// the resource to be created as IMMUTABLE.
		E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT  = 0x02,

		// Specifies static content, specified either at the creation time or afterwards via direct upload
		// API (DMA transfer or upload heap, depending on the driver). Resources with static content are
		// generally advised to have no CPU access specified, as this can prevent certain optimisations
		// and reduce the performance. Cannot be combined with DYNAMIC usage. Although STATIC|IMMUTABLE
		// is a valid combination, this effectively causes the resource to be created as IMMUTABLE.
		E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT     = 0x04,

		// Additional flag which can be combined with all three basic usage modes. It describes a resource
		// which will be used only temporarily (at most few frames), which can help the runtime to select
		// the best memory type and allocation strategy. Final effect depends on the resource type and
		// memory availability:
		// - DYNAMIC resources are most likely to end up in the host memory (RAM)
		// - for IMMUTABLE/STATIC resources, the heap type used for allocation will depend on additional
		//   flags specified for the resource (like required CPU/GPU access and explicit memory preferences).
		//   For small temporary resources, host memory can be selected due to more sophisticated allocation
		//   strategies for avoiding the fragmentation.
		E_GPU_RESOURCE_CONTENT_FLAG_TEMPORARY_BIT  = 0x08,

		// Mask with all valid CONTENT_FLAG bits set.
		E_GPU_RESOURCE_CONTENT_MASK_ALL_BITS       = 0x0F,
	};

	/// @brief
	enum EGPUResourceUsageFlags : resource_flags_value_t
	{
		E_GPU_RESOURCE_USAGE_FLAG_VERTEX_STREAM_BIT   = 0x0100,

		// Resource can be bound to one or more shader stages as an input (read-only) resource.
		// Can be used for both buffers and textures.
		E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT    = 0x0200,

		// Resource can be bound to one or more shader stages as a read/write resource supporting
		// unordered access. Can be used for both buffers and textures.
		E_GPU_RESOURCE_USAGE_FLAG_SHADER_UAV_BIT      = 0x0400,

		// Resource can be used as a source in transfer operations. Typical usage will be an upload
		// resource with CPU_WRITE access, used to write the data and copy it to the target resource.
		E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT = 0x1000,

		// Resource can be used as a target in transfer operations. This flag will be typically set
		// for all resources which can be updated via a dedicated upload resource (e.g. those without
		// CPU_WRITE access).
		E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_TARGET_BIT = 0x2000,

		// Resource can be used as a color attachment in the render target state. This enables writing
		// to such resource in the pixel shader stage. RT resources require a color-compatible format.
		E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT         = 0x4000,

		// Resource can be used as a depth/stencil attachment in the render target state. This enables
		// writing to such resource in the pixel shader stage and using it as depth/stencil buffer in
		// the depth and/or stencil tests. DS resources require a depth/stencil-compatible format.
		E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_STENCIL_BIT = 0x8000,

		// Mask with all valid USAGE_FLAG bits set.
		E_GPU_RESOURCE_USAGE_MASK_ALL      = 0xF700,
	};

	/// @brief A set of pre-defined memory bit masks for most common scenarios.
	enum EGPUResourceMemoryFlags : resource_flags_value_t
	{
		E_GPU_RESOURCE_MEMORY_MASK_DEFAULT =
			E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT |
			E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_CACHED_BIT,

		E_GPU_RESOURCE_MEMORY_MASK_CONSTANT_BUFFER =
			E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT |
			E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT |
			E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT,

		E_GPU_RESOURCE_MEMORY_MASK_VERTEX_STREAM_BUFFER_DYNAMIC =
			E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT |
			E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT |
			E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_CACHED_BIT,

		E_GPU_RESOURCE_MEMORY_MASK_VERTEX_STREAM_BUFFER_STATIC =
			E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT |
			E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_CACHED_BIT,

		E_GPU_RESOURCE_MEMORY_MASK_TRANSFER_SOURCE_BUFFER =
			E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT |
			E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_CACHED_BIT,

		E_GPU_RESOURCE_MEMORY_MASK_TRANSFER_TARGET_BUFFER =
			E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT,
	};

	struct ResourceInputDataDesc
	{
		const void * pointer = nullptr;
		gpu_memory_size_t size = 0;

		constexpr explicit operator bool() const
		{
			return pointer && ( size > 0 );
		}
	};

	struct ResourceCreateInfo
	{
		Bitmask<resource_flags_value_t> resourceFlags = 0;
		memory_align_t memoryBaseAlignment = CX_MEMORY_DEFAULT_ALIGNMENT;
		Bitmask<EGPUMemoryFlags> memoryFlags = E_GPU_RESOURCE_MEMORY_MASK_DEFAULT;
	};

	namespace Internal
	{

		template <typename TpData, bool tWritable>
		struct ResourceMapPtrTypeProxy;

		template <typename TpData>
		struct ResourceMapPtrTypeProxy<TpData, false>
		{
			using Type = TpData;
		};

		template <typename TpData>
		struct ResourceMapPtrTypeProxy<TpData, true>
		{
			using Type = const TpData;
		};

	}

	template <typename TpData, uint64 tAccessMask>
	using ResourceMapPtr = typename Internal::ResourceMapPtrTypeProxy<TpData, ( tAccessMask & E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT ) != 0>;

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_COMMON_RESOURCE_DEFS_H__
