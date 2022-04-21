
#include "VK_gpuDriver.h"
#include "VK_gpuDevice.h"

namespace ts3::gpuapi
{

	VKGPUDriver::VKGPUDriver( system::temContext * pExfSystemContext )
	: GPUDriver( pExfSystemContext )
	{ }

	VKGPUDriver::~VKGPUDriver()
	{ }

	VKGPUDriverHandle VKGPUDriver::create( const VKGPUDriverCreateInfo & pCreateInfo )
	{
		return createGPUAPIObject<VKGPUDriver>( pCreateInfo.exfSystemContext );
	}

	GPUDeviceHandle VKGPUDriver::createDevice( const GPUDeviceCreateInfo & pCreateInfo )
	{
		VKGPUDeviceCreateInfo createInfo;
		createInfo.adapterID = pCreateInfo.adapterID;
		createInfo.flags = pCreateInfo.flags;
		return VKGPUDevice::create( this, createInfo );
	}

} // namespace ts3::gpuapi
