
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_GRAPHICS_DRIVER_H__
#define __TS3DRIVER_GPUAPI_DXCOMMON_GRAPHICS_DRIVER_H__

#include "DX_prerequisites.h"
#include <ts3/gpuapi/gpuDriver.h>

namespace ts3::gpuapi
{

	class TS3GX_DXCOMMON_CLASS DXGPUDriver : public GPUDriver
	{
	public:
		explicit DXGPUDriver( SysContextHandle pSysContext ) noexcept;
		virtual ~DXGPUDriver() noexcept;
	};

}

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_GRAPHICS_DRIVER_H__
