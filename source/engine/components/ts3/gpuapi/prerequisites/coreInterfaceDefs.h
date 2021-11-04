
#pragma once

#ifndef __TS3_GPUAPI_CORE_INTERFACE_DEFS_H__
#define __TS3_GPUAPI_CORE_INTERFACE_DEFS_H__

namespace ts3
{
namespace gpuapi
{

	class GPUDriver;
	class GPUDevice;

    class GPUAPIObject : public DynamicInterface
    {
    };

    class TS3_GPUAPI_API GPUDriverChildObject : public GPUAPIObject
    {
    public:
	    GPUDriver & mGPUDriver;

	    explicit GPUDriverChildObject( GPUDriver & pGPUDriver );
	    virtual ~GPUDriverChildObject();
    };

	class TS3_GPUAPI_API GPUDeviceChildObject : public GPUAPIObject
	{
	public:
        GPUDriver & mGPUDriver;
		GPUDevice & mGPUDevice;

		explicit GPUDeviceChildObject( GPUDevice & pGPUDevice );
		virtual ~GPUDeviceChildObject();
	};

	template <typename TpClass, typename... TpArgs>
	inline Handle<TpClass> createGPUAPIObject( TpArgs && ...pArgs )
	{
		return createDynamicInterfaceObject<TpClass>( std::forward<TpArgs>( pArgs )... );
	}

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_CORE_INTERFACE_DEFS_H__
