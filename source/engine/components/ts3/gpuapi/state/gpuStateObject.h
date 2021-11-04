
#pragma once

#ifndef __TS3_GPUAPI_GPU_STATE_OBJECT__
#define __TS3_GPUAPI_GPU_STATE_OBJECT__

#include "commonGPUStateDefs.h"

namespace ts3
{
namespace gpuapi
{

	class TS3_GPUAPI_CLASS GPUStateObject : public GPUBaseObject
	{
	public:
		GPUStateObject( GPUDevice & pGPUDevice );
		virtual ~GPUStateObject();
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_GPU_STATE_OBJECT__
