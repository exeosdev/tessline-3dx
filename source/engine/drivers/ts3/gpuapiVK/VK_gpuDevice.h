
#pragma once

#ifndef __TS3DRIVER_GPUAPI_VKCOMMON_GPU_DEVICE_H__
#define __TS3DRIVER_GPUAPI_VKCOMMON_GPU_DEVICE_H__

#include "VK_prerequisites.h"
#include <ts3/gpuapi/gpuDevice.h>

namespace ts3::gpuapi
{

	struct VKGPUDeviceCreateInfo : public GPUDeviceCreateInfo
	{
	};

	/// @brief
	class TS3GX_VKCOMMON_CLASS VKGPUDevice : public GPUDevice
	{
	public:
		VkDevice const mVkDevice = nullptr;

		VKGPUDevice( VKGPUDriver * pDriver, VkDevice pVkDevice );
		virtual ~VKGPUDevice();

		static VKGPUDeviceHandle create( VKGPUDriver * pDriver, const VKGPUDeviceCreateInfo & pCreateInfo );

	private:
		virtual void initializeCommandSystem() override;

	};

}

#endif // __TS3DRIVER_GPUAPI_VKCOMMON_GPU_DEVICE_H__
