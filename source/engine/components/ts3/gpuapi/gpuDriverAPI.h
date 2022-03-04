
#pragma once

#ifndef __TS3_GPUAPI_GPU_DRIVER_API_H__
#define __TS3_GPUAPI_GPU_DRIVER_API_H__

#include "prerequisites.h"
#include <functional>

namespace ts3
{
namespace gpuapi
{

	struct GPUDriverCreateInfo;
	struct PresentationLayerCreateInfo;

	struct GPUDriverAPI
	{
		using CreateDriverFuncType = std::function<GPUDriverHandle( const GPUDriverCreateInfo & )>;
		using CreateScreenPresentationLayerFuncType = std::function<PresentationLayerHandle( GPUDevice &, const PresentationLayerCreateInfo & )>;

		CreateDriverFuncType createDriver;
		CreateScreenPresentationLayerFuncType createScreenPresentationLayer;
	};

	class GPUDriverInterface
	{
	public:
		GPUDriverInterface() = default;
		virtual ~GPUDriverInterface() = default;

		virtual GPUDriverHandle createDriver( const GPUDriverCreateInfo & pCreateInfo ) = 0;
		virtual PresentationLayerHandle createScreenPresentationLayer( GPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo ) = 0;
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_GPU_DRIVER_API_H__
