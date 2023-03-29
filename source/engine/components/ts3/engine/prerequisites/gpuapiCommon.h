
#ifndef __TS3_ENGINE_GPUAPI_COMMON_H__
#define __TS3_ENGINE_GPUAPI_COMMON_H__

#include <ts3/gpuapi/memory/commonGPUMemoryDefs.h>

namespace ts3
{

	class GpaContext
	{
	public:
		gpuapi::GPUDevice * const mGPUDevice = nullptr;

	public:
		GpaContext( gpuapi::GPUDevice & pGPUDevice )
		: mGPUDevice( &pGPUDevice )
		{}
	};

}

#endif // __TS3_ENGINE_GPUAPI_COMMON_H__
