
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_GPU_DRIVER_API_H__
#define __TS3DRIVER_GPUAPI_DX11_GPU_DRIVER_API_H__

#include "DX11_prerequisites.h"
#include <ts3/gpuapi/gpuDriverAPI.h>

namespace ts3::gpuapi
{

	class DX11GPUDriverInterface : public GPUDriverInterface
	{
	public:
		virtual GPUDriverHandle createDriver( const GPUDriverCreateInfo & pCreateInfo ) override final;
		virtual PresentationLayerHandle createScreenPresentationLayer( GPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo ) override final;
	};

}

#endif // __TS3DRIVER_GPUAPI_DX11_GPU_DRIVER_API_H__
