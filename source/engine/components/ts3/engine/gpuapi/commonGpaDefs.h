
#ifndef __TS3_ENGINE_COMMON_GPA_DEFS_H__
#define __TS3_ENGINE_COMMON_GPA_DEFS_H__

#include "../prerequisites.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>

namespace ts3
{

	enum class EGPUBufferUsagePolicy : uint64
	{
		Undefined = 0,

		ConstantBufferDynamic =
				gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT |
				gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
				bshLeft<uint64, 32>( gpuapi::E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ),

		ConstantBufferStatic =
				gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT |
				gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT,

		ConstantBufferWriteDiscard =
				gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT |
				gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
				bshLeft<uint64, 32>( gpuapi::E_GPU_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT ),

		GeometryBufferDynamic =
				gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
				gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
				gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
				bshLeft<uint64, 32>( gpuapi::E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ),

		GeometryBufferImmutable =
				gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
				gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
				gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT,

		GeometryBufferStatic =
				gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
				gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
				gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT,

		GeometryBufferStreamAppend =
				gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
				gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
				gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
				bshLeft<uint64, 32>( gpuapi::E_GPU_MEMORY_MAP_FLAG_WRITE_APPEND_BIT ),

		GeometryBufferStreamOverwrite =
				gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
				gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
				gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
				bshLeft<uint64, 32>( gpuapi::E_GPU_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT ),
	};

	namespace cxdefs
	{

		TS3_ATTR_NO_DISCARD inline constexpr uint16 getIndexDataFormatByteSize( gpuapi::EIndexDataFormat pIndexFormat ) noexcept
		{
			return gpuapi::cxdefs::getIndexDataFormatByteSize( pIndexFormat );
		}

		inline constexpr gpuapi::gpu_memory_flags_value_t getGPUBufferUsagePolicyMemoryFlags( EGPUBufferUsagePolicy pUsagePolicy )
		{
			return static_cast<gpuapi::gpu_memory_flags_value_t>( ( ( ( uint64 ) pUsagePolicy ) >> 32 ) & Limits<uint32>::maxValue );
		}

		inline constexpr gpuapi::resource_flags_value_t getGPUBufferUsagePolicyResourceFlags( EGPUBufferUsagePolicy pUsagePolicy )
		{
			return static_cast<gpuapi::resource_flags_value_t>( ( ( uint64 ) pUsagePolicy ) & Limits<uint32>::maxValue );
		}

	}
	
//	/// @brief Represents a valid GPUBuffer subregion. This is basically a sub-buffer of a larger buffer.
//	struct GPUBufferRef
//	{
//		// The buffer itself.
//		gpuapi::GPUBufferHandle buffer;
//
//		// Memory region of the buffer. Note, that the offset is not buffer-relative, but heap-relative.
//		// For an example, a GPUBufferRef referencing the whole memory of a buffer B1, would have here
//		// exactly same values as stored in B1.mResourceMemory.sourceHeapRegion.
//		gpuapi::GPUMemoryRegion memoryRegion;
//
//		// The actual region reserved to make the allocation. This is due to alignment requirements: it is
//		// possible to sub-allocate a buffer's memory using a more strict alignment than the original buffer
//		// has. To prevent extra calculation in the client code, memoryRegion always has the exact size which
//		// was requested and the actual size of the region is stored here.
//		gpuapi::GPUMemoryRegion reservedRegion;
//
//		GPUBufferRef() = default;
//
//		GPUBufferRef( std::nullptr_t )
//		: buffer{ nullptr }
//		, memoryRegion{ 0, gpuapi::cxdefs::GPU_MEMORY_SIZE_MAX }
//		{}
//
//		explicit operator bool() const
//		{
//			return buffer && !memoryRegion.empty();
//		}
//	};
//
//	struct GPUBufferUsageInfo
//	{
//		// Offset to the beginning of the free region of a buffer's memory.
//		// This is a buffer-relative offset: initially its value is zero.
//		gpuapi::gpu_memory_size_t currentAllocOffset;
//
//		// The size of a memory available for allocation (in bytes).
//		// Initially its value is the size of the whole GPUBuffer.
//		gpuapi::gpu_memory_size_t availableMemorySize;
//
//		// Number of HWBuffers allocated from the referenced GPUBuffer.
//		uint32 allocatedSubRegionCount;
//	};
//
//	struct HWBufferMetrics
//	{
//		gpuapi::gpu_memory_size_t bufferSize;
//	};
//
//	struct HWGeometryBufferMetrics : public HWBufferMetrics
//	{
//		uint32 instanceDataStepRate;
//		uint32 vertexCount;
//		uint32 vertexSize;
//	};
//
//	struct HWIndexBufferMetrics : public HWBufferMetrics
//	{
//		uint32 indexCount;
//		uint32 indexSize;
//	};

} // namespace ts3

#endif // __TS3_ENGINE_COMMON_GPA_DEFS_H__
