
#pragma once

#ifndef __TS3_GPUAPI_GPU_STATE_OBJECT__
#define __TS3_GPUAPI_GPU_STATE_OBJECT__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	class TS3_GPUAPI_CLASS GPUStateObject : public GPUBaseObject
	{
	public:
		GPUStateObject( GPUDevice & pGPUDevice );
		virtual ~GPUStateObject();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_STATE_OBJECT__
