
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_GPU_DEVICE_H__
#define __TS3DRIVER_GPUAPI_DXCOMMON_GPU_DEVICE_H__

#include "DX_prerequisites.h"
#include <ts3/gpuapi/gpuDevice.h>

namespace ts3
{
namespace gpuapi
{

	class DXGPUDevice : public GPUDevice
	{
	public:
		ComPtr<IDXGIFactory2> const mDXGIFactory2;
		ComPtr<IDXGIDebug> const mDXGIDebug;
		ComPtr<IDXGIInfoQueue> const mDXGIInfoQueue;

		explicit DXGPUDevice( DXGPUDriver & pDXGPUDriver, ComPtr<IDXGIFactory2> pDXGIFactory2 );
		virtual ~DXGPUDevice();
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_GPU_DEVICE_H__
