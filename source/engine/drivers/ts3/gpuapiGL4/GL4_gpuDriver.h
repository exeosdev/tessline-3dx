
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

		virtual EGPUDriverID queryGPUDriverID() const noexcept override;

		static GL4GPUDriverHandle create( const GL4GPUDriverCreateInfo & pCreateInfo );

	private:
		virtual DisplayManagerHandle _drvCreateDefaultDisplayManager() override;

		virtual GPUDeviceHandle _drvCreateDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GL4_GPU_DRIVER__
