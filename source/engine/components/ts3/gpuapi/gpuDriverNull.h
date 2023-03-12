
#pragma once

#ifndef __TS3_GPUAPI_GPU_DRIVER_NULL_H__
#define __TS3_GPUAPI_GPU_DRIVER_NULL_H__

#include "gpuDriver.h"

namespace ts3::gpuapi
{

	class GPUDriverNull : public GPUDriver
	{
	public:
		explicit GPUDriverNull() noexcept
		: GPUDriver( nullptr )
		{}

		virtual ~GPUDriverNull() noexcept = default;

		TS3_ATTR_NO_DISCARD virtual EGPUDriverID queryGPUDriverID() const noexcept override final
		{
			return EGPUDriverID::GDINull;
		}

		TS3_ATTR_NO_DISCARD virtual bool isNullDriver() const noexcept override final
		{
			return true;
		}

	private:
		virtual DisplayManagerHandle _drvCreateDefaultDisplayManager() override final
		{
			return nullptr;
		}

		virtual GPUDeviceHandle _drvCreateDevice( const GPUDeviceCreateInfo & pCreateInfo ) override final
		{
			return nullptr;
		}
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_DRIVER_NULL_H__
