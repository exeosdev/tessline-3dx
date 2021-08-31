
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLES3_GRAPHICS_DRIVER_H__
#define __TS3DRIVER_GPUAPI_GLES3_GRAPHICS_DRIVER_H__

#include "GLES3_prerequisites.h"
#include <ts3/gpuapiGL/GL_gpuDriver.h>

namespace ts3::gpuapi
{

	struct GLES3GPUDriverCreateInfo : public GPUDriverCreateInfo
	{
	};

	class TS3GX_GLES3_CLASS GLES3GPUDriver final : public GLGPUDriver
	{
	public:
		explicit GLES3GPUDriver( ts3::SysGLDriver * pExfGLDriver );
		virtual ~GLES3GPUDriver();

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) override;

		static GLES3GPUDriverHandle create( const GLES3GPUDriverCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_GLES3_GRAPHICS_DRIVER_H__
