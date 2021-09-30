
#pragma once

#ifndef __TS3_GPUAPI_GPU_DRIVER_H__
#define __TS3_GPUAPI_GPU_DRIVER_H__

#include "prerequisites.h"
#include <ts3/system/sysContextNative.h>

namespace ts3::gpuapi
{

	struct GPUDeviceCreateInfo;

	struct GPUDriverCreateInfo
	{
		system::SysContextHandle sysContext = nullptr;
		system::SysContextCreateInfo sysContextCreateInfo;
		Bitmask<GPUDriverConfigFlags> configFlags = GPU_DRIVER_CONFIG_FLAGS_DEFAULT;
	};

	class TS3_GPUAPI_CLASS GPUDriver : public DynamicInterface
	{
	public:
		system::SysContextHandle const mSysContext;

	public:
		explicit GPUDriver( system::SysContextHandle pSysContext ) noexcept;
		virtual ~GPUDriver() noexcept;

		virtual DisplayManagerHandle createDefaultDisplayManager() = 0;

		virtual GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo ) = 0;

		virtual EGPUDriverID queryGPUDriverID() const = 0;

		Bitmask<GPUDriverConfigFlags> getConfigFlags() const;

		bool isDebugFunctionalityRequested() const;

	protected:
		void setConfigFlags( Bitmask<GPUDriverConfigFlags> pConfigFlags );

	private:
		Bitmask<GPUDriverConfigFlags> _configFlags;
	};

	inline Bitmask<GPUDriverConfigFlags> GPUDriver::getConfigFlags() const
	{
		return _configFlags;
	}

	inline bool GPUDriver::isDebugFunctionalityRequested() const
	{
		return _configFlags.isSet( GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT );
	}

}

#endif // __TS3_GPUAPI_GPU_DRIVER_H__
