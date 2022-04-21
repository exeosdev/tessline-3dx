
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_GPU_DRIVER__
#define __TS3DRIVER_GPUAPI_DXCOMMON_GPU_DRIVER__

#include "DX_prerequisites.h"
#include <ts3/gpuapi/gpuDriver.h>

namespace ts3::gpuapi
{

	class TS3GX_DXCOMMON_CLASS DXGPUDriver : public GPUDriver
	{
	public:
		explicit DXGPUDriver( system::SysContextHandle pSysContext ) noexcept;
		virtual ~DXGPUDriver() noexcept;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_GPU_DRIVER__
