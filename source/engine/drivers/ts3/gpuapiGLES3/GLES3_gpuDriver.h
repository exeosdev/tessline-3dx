
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLES3_GPU_DRIVER__
#define __TS3DRIVER_GPUAPI_GLES3_GPU_DRIVER__

#include "GLES3_prerequisites.h"
#include <ts3/gpuapiGL/GL_gpuDriver.h>

namespace ts3
{
namespace gpuapi
{

	struct GLES3GPUDriverCreateInfo : public GLGPUDriverCreateInfo
	{
	};

	class TS3GX_GLES3_CLASS GLES3GPUDriver final : public GLGPUDriver
	{
	public:
	explicit GLES3GPUDriver( system::OpenGLSystemDriverHandle pSysGLDriver );
		virtual ~GLES3GPUDriver();

		virtual DisplayManagerHandle createDefaultDisplayManager() override;

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;

		virtual EGPUDriverID queryGPUDriverID() const override;

		static GLES3GPUDriverHandle create( const GLES3GPUDriverCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLES3_GPU_DRIVER__
