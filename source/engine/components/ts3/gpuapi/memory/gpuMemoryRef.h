
#pragma once

#ifndef __TS3_GPUAPI_GPU_MEMORY_POOL_H__
#define __TS3_GPUAPI_GPU_MEMORY_POOL_H__

#include "commonGPUMemoryDefs.h"
#include <ts3/stdext/syncCommon.h>

namespace ts3::gpuapi
{

	class GPUMemoryRef
	{
	public:
		GPUMemoryPool * const mSourcePool;

		GPUMemoryRef( GPUMemoryPool & pSourcePool, const GPUMemoryRegion & pPoolSubRegion );
		~GPUMemoryRef();

		void lockMemory();

		bool tryLockMemory();

		void unlockMemory();

		TS3_PCL_ATTR_NO_DISCARD bool isEmpty() const;

		TS3_PCL_ATTR_NO_DISCARD bool isMemoryLocked() const;

	private:
		GPUMemoryRegion _poolSubRegion;
		std::shared_lock<SharedSpinLock> _poolMemoryLock;
		std::atomic<uint32_t> _poolMemoryLockStatus;
	};

}

#endif // __TS3_GPUAPI_GPU_MEMORY_POOL_H__

