
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_GPU_DRIVER__
#define __TS3DRIVER_GPUAPI_DX11_GPU_DRIVER__

#include "DX11_prerequisites.h"
#include <ts3/gpuapiDX/DX_gpuDriver.h>

namespace ts3::gpuapi
{

	struct DX11GPUDriverCreateInfo : public GPUDriverCreateInfo
	{
	};

	class TS3GX_DX11_CLASS DX11GPUDriver final : public DXGPUDriver
	{
	public:
		explicit DX11GPUDriver( system::SysContextHandle pSysContext ) noexcept;
		virtual ~DX11GPUDriver() noexcept;

		virtual DisplayManagerHandle createDefaultDisplayManager() override;

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;

		virtual EGPUDriverID queryGPUDriverID() const override;

		static DX11GPUDriverHandle create( const DX11GPUDriverCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_DX11_GPU_DRIVER__
