
#pragma once

#ifndef __TS3_GPUAPI_GPU_MEMORY_POOL_H__
#define __TS3_GPUAPI_GPU_MEMORY_POOL_H__

#include "commonGPUMemoryDefs.h"
#include <ts3/core/sync/spinLock.h>
#include <ts3/core/sync/syncInterface.h>

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

		TS3_PCL_ATTR_FUNC_NO_DISCARD bool empty() const;

		TS3_PCL_ATTR_FUNC_NO_DISCARD bool isMemoryLocked() const;

	private:
		GPUMemoryRegion _poolSubRegion;
		sync::AutoSharedLock<sync::SharedSpinLock> _poolMemoryLock;
		std::atomic<uint32_t> _poolMemoryLockStatus;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_MEMORY_POOL_H__

