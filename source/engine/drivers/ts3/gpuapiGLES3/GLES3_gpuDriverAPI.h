
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLES3_GRAPHICS_DRIVER_API_H__
#define __TS3DRIVER_GPUAPI_GLES3_GRAPHICS_DRIVER_API_H__

#include "GLES3_prerequisites.h"
#include <ts3/gpuapi/gpuDriverAPI.h>

namespace ts3
{
namespace gpuapi
{

	class GLES3GPUDriverInterface : public GPUDriverInterface
	{
	public:
		virtual GPUDriverHandle createDriver( const GPUDriverCreateInfo & pCreateInfo ) override final;
		virtual PresentationLayerHandle createScreenPresentationLayer( GPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo ) override final;
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLES3_GRAPHICS_DRIVER_API_H__
