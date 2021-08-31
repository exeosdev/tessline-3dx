
#include "VK_gpuDriver.h"
#include "VK_gpuDevice.h"

namespace ts3::gpuapi
{

	VKGPUDriver::VKGPUDriver( ts3::SysContext * pExfSysContext )
	: GPUDriver( pExfSysContext )
	{ }

	VKGPUDriver::~VKGPUDriver()
	{ }

	VKGPUDriverHandle VKGPUDriver::create( const VKGPUDriverCreateInfo & pCreateInfo )
	{
		return createGPUAPIObject<VKGPUDriver>( pCreateInfo.exfSysContext );
	}

	GPUDeviceHandle VKGPUDriver::createDevice( const GPUDeviceCreateInfo & pCreateInfo )
	{
		VKGPUDeviceCreateInfo createInfo;
		createInfo.adapterID = pCreateInfo.adapterID;
		createInfo.flags = pCreateInfo.flags;
		return VKGPUDevice::create( this, createInfo );
	}

}
