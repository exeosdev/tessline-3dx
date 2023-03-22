
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

		virtual EGPUDriverID queryGPUDriverID() const noexcept override final;

		static DX11GPUDriverHandle create( const DX11GPUDriverCreateInfo & pCreateInfo );

	private:
		virtual DisplayManagerHandle _drvCreateDefaultDisplayManager() override;

		virtual GPUDeviceHandle _drvCreateDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_GPU_DRIVER__
