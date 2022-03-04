
#pragma once

#ifndef __TS3_GPUAPI_GPU_MEMORY_POOL_H__
#define __TS3_GPUAPI_GPU_MEMORY_POOL_H__

#include "commonGPUMemoryDefs.h"
#include <ts3/core/sync/spinLock.h>

namespace ts3
{
namespace gpuapi
{

	class GPUMemoryAllocator;
	class GPUMemoryHeap;
	class GPUMemoryRef;

	class GPUMemoryPool
	{
		friend class GPUMemoryAllocator;
		friend class GPUMemoryRef;

	public:
		using MemoryLock = sync::SharedSpinLock;

		gpu_memory_pool_id_t const mPoolID;
		gpu_memory_size_t const mPoolSize;
		GPUMemoryHeap * const mSourceHeap;
		gpu_memory_heap_id_t const mSourceHeapID;

	public:
		GPUMemoryPool();
		virtual ~GPUMemoryPool();

		gpu_memory_size_t getCurrentUsage() const;

	protected:
		void setCurrentUsage( gpu_memory_size_t pUsageInBytes );

		MemoryLock & getMemoryLock();

	private:
		GPUMemoryRegion _sourceHeapRegion;
		std::atomic<gpu_memory_size_t> _currentUsage;
		MemoryLock _memoryLock;
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_GPU_MEMORY_POOL_H__

