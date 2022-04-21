
#pragma once

#ifndef __TS3DRIVER_GPUAPI_VK10_GPU_DRIVER_API_H__
#define __TS3DRIVER_GPUAPI_VK10_GPU_DRIVER_API_H__

#include "ts3/gpuapiVK/VK_prerequisites.h"
#include <ts3/gpuapi/gpuDriverAPI.h>

namespace ts3::gpuapi
{

	class VK10GPUDriverInterface : public GPUDriverInterface
	{
	public:
		virtual GPUDriverHandle createDriver( const GPUDriverCreateInfo & pCreateInfo ) override final;
		virtual PresentationLayerHandle createScreenPresentationLayer( GPUDevice * pDevice, const PresentationLayerCreateInfo & pCreateInfo ) override final;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_VK10_GPU_DRIVER_API_H__
