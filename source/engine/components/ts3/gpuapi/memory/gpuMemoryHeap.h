
#pragma once

#ifndef __TS3_GPUAPI_GPU_MEMORY_HEAP_H__
#define __TS3_GPUAPI_GPU_MEMORY_HEAP_H__

#include "commonGPUMemoryDefs.h"

namespace ts3::GpuAPI
{

	struct GPUMemoryPoolMetrics
	{
		memory_align_t baseAlignment;
		gpu_memory_size_t totalSizeBase;
		gpu_memory_size_t totalSizeMB;
	};

	struct GPUMemoryHeapProperties
	{
		gpu_memory_heap_id_t heapID;
		Bitmask<EGPUMemoryFlags> memoryFlags;
		GPUMemoryPoolMetrics heapMetrics;
	};

	class GPUMemoryHeap
	{
	public:
		GPUMemoryHeapProperties const mHeapProperties;

	public:
		GPUMemoryHeap();
		virtual ~GPUMemoryHeap();
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_GPU_MEMORY_HEAP_H__
