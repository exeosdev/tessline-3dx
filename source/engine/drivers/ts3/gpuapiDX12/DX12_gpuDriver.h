
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX12_GPU_DRIVER__
#define __TS3DRIVER_GPUAPI_DX12_GPU_DRIVER__

#include "DX12_prerequisites.h"
#include <ts3/gpuapiDX/DX_gpuDriver.h>


namespace ts3::gpuapi
{

	struct DX12GPUDriverCreateInfo : public GPUDriverCreateInfo
	{
	};

	class TS3GX_DX12_CLASS DX12GPUDriver final : public DXGPUDriver
	{
	public:
		explicit DX12GPUDriver( system::temContext * pExfSystemContext );
		virtual ~DX12GPUDriver();

		virtual DisplayManagerHandle createDefaultDisplayManager() override;

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;

		static DX12GPUDriverHandle create( const DX12GPUDriverCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_DX12_GPU_DRIVER__
