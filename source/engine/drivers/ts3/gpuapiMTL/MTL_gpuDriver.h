
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_DRIVER_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_DRIVER_H__

#include "MTL_prerequisites.h"
#include <ts3/gpuapi/gpuDriver.h>

namespace ts3::gpuapi
{

	struct MTLGPUDriverCreateInfo : public GPUDriverCreateInfo
	{
	};

	/// @brief Middle-layer OpenMTL driver class. Implements features which are common for all MTL drivers (core, ES).
	class MetalGPUDriver : public GPUDriver
	{
	public:
		system::MetalSystemDriverHandle const mSysMetalDriver;

	public:
		explicit MetalGPUDriver( system::MetalSystemDriverHandle pSysMetalDriver );
		virtual ~MetalGPUDriver();

	protected:
		/// @brief Helper method. Initializes EXF OpenMTL driver component.
		/// Used by specific sub-classes (drivers) when a driver is created.
		static system::MetalSystemDriverHandle initializeSysMTLDriver( system::SysContextHandle pSysContext );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_DRIVER_H__
