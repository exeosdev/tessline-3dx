
#pragma once

#ifndef __TS3_GPUAPI_CORE_INTERFACE_DEFS_H__
#define __TS3_GPUAPI_CORE_INTERFACE_DEFS_H__

namespace ts3::gpuapi
{

	class GPUDriver;
	class GPUDevice;

	/// @brief
    class GPUAPIObject : public DynamicInterface
    {
    };

	/// @brief
    class TS3_GPUAPI_API GPUDriverChildObject : public GPUAPIObject
    {
    public:
	    GPUDriver & mGPUDriver;

	    explicit GPUDriverChildObject( GPUDriver & pGPUDriver );
	    virtual ~GPUDriverChildObject();
    };

	/// @brief
	class TS3_GPUAPI_API GPUDeviceChildObject : public GPUAPIObject
	{
	public:
        GPUDriver & mGPUDriver;
		GPUDevice & mGPUDevice;

	public:
		explicit GPUDeviceChildObject( GPUDevice & pGPUDevice );
		virtual ~GPUDeviceChildObject();

		virtual UniqueGPUObjectID queryObjectID() const;
	};

	template <typename TClass, typename... TArgs>
	inline GpaHandle<TClass> createGPUAPIObject( TArgs && ...pArgs )
	{
		return createDynamicInterfaceObject<TClass>( std::forward<TArgs>( pArgs )... );
	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_CORE_INTERFACE_DEFS_H__
