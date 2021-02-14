
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX12_GRAPHICS_DRIVER_H__
#define __TS3DRIVER_GPUAPI_DX12_GRAPHICS_DRIVER_H__

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
		explicit DX12GPUDriver( ts3::SystemContext * pExfSystemContext );
		virtual ~DX12GPUDriver();

		virtual DisplayManagerHandle createDefaultDisplayManager() override;

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;

		static DX12GPUDriverHandle create( const DX12GPUDriverCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_DX12_GRAPHICS_DRIVER_H__
