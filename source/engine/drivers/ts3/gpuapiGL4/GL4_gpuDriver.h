
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GL4_GRAPHICS_DRIVER_H__
#define __TS3DRIVER_GPUAPI_GL4_GRAPHICS_DRIVER_H__

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
		explicit GL4GPUDriver( SysGLDriverHandle pSysGLDriver );
		virtual ~GL4GPUDriver();

		virtual DisplayManagerHandle createDefaultDisplayManager() override;

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;

		virtual EGPUDriverID queryGPUDriverID() const override;

		static GL4GPUDriverHandle create( const GL4GPUDriverCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_GL4_GRAPHICS_DRIVER_H__
