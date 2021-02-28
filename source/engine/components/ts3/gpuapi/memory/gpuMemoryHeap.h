
#pragma once

#ifndef __TS3_GPUAPI_GPU_MEMORY_HEAP_H__
#define __TS3_GPUAPI_GPU_MEMORY_HEAP_H__

#include "commonGPUMemoryDefs.h"

namespace ts3::gpuapi
{

	struct GPUMemoryHeapMetrics
	{
		gpu_memory_heap_id_t heapID;
		gpu_memory_align_t memoryBaseAlignment;
		Bitmask<EMemoryFlags> memoryFlags;
		gpu_memory_size_t
	};

	class GPUMemoryHeap
	{
	};

}

#endif // __TS3_GPUAPI_GPU_MEMORY_HEAP_H__

