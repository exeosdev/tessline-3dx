
#ifndef __TS3_ENGINE_COMMON_GPU_DEFS_H__
#define __TS3_ENGINE_COMMON_GPU_DEFS_H__

#include "../prerequisites.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>

namespace ts3
{

	/// @brief Represents a valid GPUBuffer subregion. This is basically a sub-buffer of a larger buffer.
	struct GPUBufferRef
	{
		// The buffer itself.
		gpuapi::GPUBufferHandle buffer;

		// Memory region of the buffer. Note, that the offset is not buffer-relative, but heap-relative.
		// For an example, a GPUBufferRef referencing the whole memory of a buffer B1, would have here
		// exactly same values as stored in B1.mResourceMemory.sourceHeapRegion.
		gpuapi::MemoryRegion memoryRegion;

		// The actual region reserved to make the allocation. This is due to alignment requirements: it is
		// possible to sub-allocate a buffer's memory using a more strict alignment than the original buffer
		// has. To prevent extra calculation in the client code, memoryRegion always has the exact size which
		// was requested and the actual size of the region is stored here.
		gpuapi::MemoryRegion reservedRegion;

		GPUBufferRef() = default;

		GPUBufferRef( std::nullptr_t )
		: buffer{ nullptr }
		, memoryRegion{ 0, gpuapi::cxMemorySizeMax }
		{}

		explicit operator bool() const
		{
			return buffer && !memoryRegion.empty();
		}
	};

	struct GPUBufferUsageInfo
	{
		// Offset to the beginning of the free region of a buffer's memory.
		// This is a buffer-relative offset: initially its value is zero.
		gpuapi::memory_offset_t currentAllocOffset;

		// The size of a memory available for allocation (in bytes).
		// Initially its value is the size of the whole GPUBuffer.
		gpuapi::memory_size_t availableMemorySize;

		// Number of HWBuffers allocated from the referenced GPUBuffer.
		uint32 allocatedSubRegionCount;
	};

	struct GenericHWBufferMetrics
	{
		gpuapi::memory_size_t bufferSize;
	};

	struct VertexHWBufferMetrics : public GenericHWBufferMetrics
	{
		uint32 instanceDataStepRate;
		uint32 vertexCount;
		uint32 vertexSize;
	};

	struct IndexHWBufferMetrics : public GenericHWBufferMetrics
	{
		uint32 indexCount;
		uint32 indexSize;
	};

}

#endif // __TS3_ENGINE_COMMON_GPU_DEFS_H__
