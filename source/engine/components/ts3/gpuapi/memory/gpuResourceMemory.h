
#pragma once

#ifndef __TS3_GPUAPI_GPU_RESOURCE_MEMORY_H__
#define __TS3_GPUAPI_GPU_RESOURCE_MEMORY_H__

#include "commonGPUMemoryDefs.h"

namespace ts3::gpuapi
{

	class GPUResourceMemory
	{
	public:
		virtual void lock();
		virtual void unlock();
	};

}

#endif // __TS3_GPUAPI_GPU_RESOURCE_MEMORY_H__

