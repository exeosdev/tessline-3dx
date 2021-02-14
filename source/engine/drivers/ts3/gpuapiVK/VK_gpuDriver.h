
#pragma once

#ifndef __TS3DRIVER_GPUAPI_VKCOMMON_GRAPHICS_DRIVER_H__
#define __TS3DRIVER_GPUAPI_VKCOMMON_GRAPHICS_DRIVER_H__

#include "VK_prerequisites.h"
#include <ts3/gpuapi/gpuDriver.h>

namespace ts3::gpuapi
{

	struct VKGPUDriverCreateInfo : public GPUDriverCreateInfo
	{
	};

	class TS3GX_VKCOMMON_CLASS VKGPUDriver : public GPUDriver
	{
	public:
		VkInstance const mVkInstance = nullptr;
		
		explicit VKGPUDriver( ts3::SystemContext * pExfSystemContext );
		virtual ~VKGPUDriver();

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;

		static VKGPUDriverHandle create( const VKGPUDriverCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_VKCOMMON_GRAPHICS_DRIVER_H__
