
#pragma once

#ifndef __TS3_GPUAPI_COMMON_MEMORY_DEFS_H__
#define __TS3_GPUAPI_COMMON_MEMORY_DEFS_H__

#include "../prerequisites.h"
#include <ts3/core/memory/commonMemoryDefs.h>

namespace ts3::gpuapi
{

    class GPUMemoryHeap;
    class GPUMemoryPool;

    using gpu_memory_diff_t = int64;
	using gpu_memory_size_t = uint64;
	using gpu_memory_flags_value_t = uint32;

    using gpu_memory_heap_id_t = uint64;
    using gpu_memory_pool_id_t = uint64;

    using GPUMemoryRegion = Region<gpu_memory_size_t>;
    using GPUMemoryRange = GPUMemoryRegion::Range;

    constexpr gpu_memory_size_t cxGPUMemoryOffsetInvalid = ts3::Limits<gpu_memory_size_t>::maxValue;

    constexpr gpu_memory_size_t cxGPUMemorySizeMax = ts3::Limits<gpu_memory_size_t>::maxValue;

	/// @brief Flags representing various properties of host/device memory pools like access and heap properties.
	enum EGPUMemoryFlags : gpu_memory_flags_value_t
	{
		// Memory has a READ access granted to the CPU.
		E_GPU_MEMORY_ACCESS_FLAG_CPU_READ_BIT   = 0x0001,

		// Memory has a WRITE access granted to the CPU.
		// Note: if a region of device-local memory has a WRITE access, but the READ access is (or may be) missing,
		// the app must ensure it does not read any content of the memory (including potential compiler-issued code).
		// This can cause a significant performance penalty.
		E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT  = 0x0002,

		// Memory has a READ access granted to the GPU.
		E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT   = 0x0004,

		// Memory has a WRITE access granted to the GPU.
		E_GPU_MEMORY_ACCESS_FLAG_GPU_WRITE_BIT  = 0x0008,

		// A helper constant yielding both READ and WRITE CPU access.
		E_GPU_MEMORY_ACCESS_MASK_CPU_READ_WRITE = E_GPU_MEMORY_ACCESS_FLAG_CPU_READ_BIT | E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT,

		// Mask with all valid ACCESS_FLAG_CPU bits set.
		E_GPU_MEMORY_ACCESS_MASK_CPU_ALL        = E_GPU_MEMORY_ACCESS_MASK_CPU_READ_WRITE,

		// A helper constant yielding both READ and WRITE GPU access.
		E_GPU_MEMORY_ACCESS_MASK_GPU_READ_WRITE = E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT | E_GPU_MEMORY_ACCESS_FLAG_GPU_WRITE_BIT,

		// Mask with all valid ACCESS_FLAG_GPU bits set.
		E_GPU_MEMORY_ACCESS_MASK_GPU_ALL        = E_GPU_MEMORY_ACCESS_MASK_GPU_READ_WRITE,

		// Mask with all valid ACCESS_FLAG bits set.
		E_GPU_MEMORY_ACCESS_MASK_ALL            = E_GPU_MEMORY_ACCESS_MASK_CPU_ALL | E_GPU_MEMORY_ACCESS_MASK_GPU_ALL,

		// Memory heap is CPU-cached, which means all host-side writes are not visible to the device until
		// an explicit flush is performed on the modified memory range.
		E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_CACHED_BIT     = 0x0010,

		// Memory heap is CPU-coherent. All host accesses to this memory is automatically made visible to the device.
		// Mapped memory without this flag will require an explicit Flush() to make CPU writes visible to the GPU.
		E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT   = 0x0020,

		// Memory heap is GPU-coherent. All device accesses to this memory is automatically made visible to the host.
		// Mapped memory without this flag will require an explicit Invalidate() to make GPU writes visible to the CPU.
		E_GPU_MEMORY_HEAP_PROPERTY_FLAG_GPU_COHERENT_BIT   = 0x0040,

		// Memory heap supports persistent mapping. It can be mapped once and such map pointer can be used as long as
		// the app needs to. Persistent mapping may require additional visibility control depending on caching/coherency.
		E_GPU_MEMORY_HEAP_PROPERTY_FLAG_PERSISTENT_MAP_BIT = 0x0080,

		// Mask with all valid HEAP_PROPERTY_FLAG bits set.
		E_GPU_MEMORY_HEAP_PROPERTY_MASK_ALL                = 0x00F0,
	};

	/// @brief Valid flags
	enum EGPUMemoryMapFlags : gpu_memory_flags_value_t
	{
		// Memory is mapped with read-only permission.
		E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_BIT       = E_GPU_MEMORY_ACCESS_FLAG_CPU_READ_BIT,

		// Memory is mapped with both read and write permissions.
		E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_WRITE_BIT = E_GPU_MEMORY_ACCESS_MASK_CPU_READ_WRITE,

		// Resource is mapped with write-only permission.
		E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT      = E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT,

		// Resource is mapped with write-only permission. However, application must ensure, that sections of the
		// resource, that could be potentially in use, will not be affected. Write operations should be performed
		// only on uninitialized sections of the resource. Since driver assumes that guarantee, this access mode
		// is more efficient than E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT, because it does not involve a CPU<>GPU syncs.
		// Modifying sections which are in use is an undefined behaviour.
		E_GPU_MEMORY_MAP_FLAG_WRITE_APPEND_BIT      = 0x1000 | E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT,

		// Resource is mapped with write-only permission. After resource is mapped, all its data is discarded and
		// becomes undefined. This access can only be used in case of resources with dynamic usage.
		E_GPU_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT  = 0x2000 | E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT,
	};

	enum class EGPUMemoryMapMode : uint32
	{
		// @See E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_BIT
		ReadOnly = E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_BIT,

		// @See E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_WRITE_BIT
		ReadWrite = E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_WRITE_BIT,

		// @See E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT
		WriteDefault = E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT,

		// @See E_GPU_MEMORY_MAP_FLAG_WRITE_APPEND_BIT
		WriteAppend = E_GPU_MEMORY_MAP_FLAG_WRITE_APPEND_BIT,

		// @See E_GPU_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT
		WriteInvalidate = E_GPU_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT,
	};

	struct ResourceMemoryInfo
	{
        memory_align_t baseAlignment;
		Bitmask<EGPUMemoryFlags> memoryFlags;
		gpu_memory_heap_id_t sourceHeapID;
		GPUMemoryRegion sourceHeapRegion;
	};

	struct ResourceMappedMemory
	{
		const ResourceMemoryInfo * sourceMemory = nullptr;
		GPUMemoryRegion mappedRegion;
		void * pointer;
		Bitmask<EGPUMemoryMapFlags> memoryMapFlags;

		constexpr explicit operator bool () const
		{
			return sourceMemory != nullptr;
		}
	};

	inline bool checkMemoryMapAccess( EGPUMemoryMapMode pMapMode, Bitmask<EGPUMemoryFlags> pMemoryFlags )
	{
		auto mapRequestedAccessFlags = static_cast<uint32>( pMapMode ) & E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_WRITE_BIT;
		return pMemoryFlags.isSet( mapRequestedAccessFlags );
	}

} // namespace gpuapi

#endif // __TS3_GPUAPI_COMMON_MEMORY_DEFS_H__
