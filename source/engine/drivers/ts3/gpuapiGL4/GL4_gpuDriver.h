
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GL4_GPU_DRIVER__
#define __TS3DRIVER_GPUAPI_GL4_GPU_DRIVER__

#include "GL4_prerequisites.h"
#include <ts3/gpuapiGL/GL_gpuDriver.h>

namespace ts3::gpuapi
{

	struct GL4GPUDriverCreateInfo : public GLGPUDriverCreateInfo
	{
	};

	class TS3GX_GL4_CLASS GL4GPUDriver final : public GLGPUDriver
	{
	public:
		explicit GL4GPUDriver( system::OpenGLSystemDriverHandle pSysGLDriver );
		virtual ~GL4GPUDriver();

		virtual DisplayManagerHandle createDefaultDisplayManager() override;

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;

		virtual EGPUDriverID queryGPUDriverID() const override;

		static GL4GPUDriverHandle create( const GL4GPUDriverCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GL4_GPU_DRIVER__
