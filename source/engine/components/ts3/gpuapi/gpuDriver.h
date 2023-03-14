
#pragma once

#ifndef __TS3_GPUAPI_GPU_DRIVER_H__
#define __TS3_GPUAPI_GPU_DRIVER_H__

#include "prerequisites.h"
#include <ts3/system/sysContext.h>

namespace ts3::gpuapi
{

	struct GPUDeviceCreateInfo;

	struct GPUDriverCreateInfo
	{
		system::SysContextHandle sysContext = nullptr;
		system::SysContextCreateInfo sysContextCreateInfo;
		Bitmask<EGPUDriverConfigFlags> configFlags = GPU_DRIVER_CONFIG_FLAGS_DEFAULT;
	};

	class TS3_GPUAPI_CLASS GPUDriver : public DynamicInterface
	{
	public:
		system::SysContextHandle const mSysContext;

	public:
		explicit GPUDriver( system::SysContextHandle pSysContext ) noexcept;
		virtual ~GPUDriver() noexcept;

		TS3_ATTR_NO_DISCARD virtual EGPUDriverID queryGPUDriverID() const noexcept = 0;

		TS3_ATTR_NO_DISCARD virtual bool isNullDriver() const noexcept;

		Bitmask<EGPUDriverConfigFlags> getConfigFlags() const;

		bool isDebugFunctionalityRequested() const;

		DisplayManagerHandle createDefaultDisplayManager();

		GPUDeviceHandle createDevice( const GPUDeviceCreateInfo & pCreateInfo );

		static GPUDriver & nullDriver();

	protected:
		void setConfigFlags( Bitmask<EGPUDriverConfigFlags> pConfigFlags );

	private:
		virtual DisplayManagerHandle _drvCreateDefaultDisplayManager() = 0;

		virtual GPUDeviceHandle _drvCreateDevice( const GPUDeviceCreateInfo & pCreateInfo ) = 0;


	private:
		Bitmask<EGPUDriverConfigFlags> _configFlags;
	};

	inline Bitmask<EGPUDriverConfigFlags> GPUDriver::getConfigFlags() const
	{
		return _configFlags;
	}

	inline bool GPUDriver::isDebugFunctionalityRequested() const
	{
		return _configFlags.isSet( E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT );
	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_DRIVER_H__
