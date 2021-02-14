
#ifndef __TS3_ENGINE_GPU_API_COMMON_H__
#define __TS3_ENGINE_GPU_API_COMMON_H__

#include "../prerequisites.h"
#include <ts3/gpuapi/memory/commonMemoryDefs.h>
#include <ts3/gpuapi/state/pipelineStateDesc.h>

namespace ts3
{

	using gpu_memory_align_t = gpuapi::memory_align_t;
	using gpu_memory_offset_t = gpuapi::memory_offset_t;
	using gpu_memory_size_t = gpuapi::memory_size_t;

	using GPUMemoryRange = gpuapi::MemoryRange;
	using GPUMemoryRegion = gpuapi::MemoryRegion;

	using GPUMemoryRef = GPUMemoryRegion;

}

#endif // __TS3_ENGINE_GPU_API_COMMON_H__
