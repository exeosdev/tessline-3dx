
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DRIVER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DRIVER_H__

#include "GL_prerequisites.h"
#include <ts3/gpuapi/gpuDriver.h>

namespace ts3::gpuapi
{

	struct GLGPUDriverCreateInfo : public GPUDriverCreateInfo
	{
	};

	/// @brief Middle-layer OpenGL driver class. Implements features which are common for all GL drivers (core, ES).
	class TS3GX_GL_CLASS GLGPUDriver : public GPUDriver
	{
	public:
		system::OpenGLSystemDriverHandle const mSysGLDriver;

	public:
		explicit GLGPUDriver( system::OpenGLSystemDriverHandle pSysGLDriver );
		virtual ~GLGPUDriver();

	protected:
		/// @brief Helper method. Initializes EXF OpenGL driver component.
		/// Used by specific sub-classes (drivers) when a driver is created.
		static system::OpenGLSystemDriverHandle initializeSysGLDriver( system::SysContextHandle pSysContext );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_DRIVER_H__
