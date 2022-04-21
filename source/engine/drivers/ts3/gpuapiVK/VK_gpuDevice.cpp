
#include "VK_gpuDevice.h"
#include "VK_gpuDriver.h"

namespace ts3::gpuapi
{

	VKGPUDevice::VKGPUDevice( VKGPUDriver * pDriver, VkDevice pVkDevice )
	: GPUDevice( pDriver )
	, mVkDevice( pVkDevice )
	{ }

	VKGPUDevice::~VKGPUDevice() = default;

	VKGPUDeviceHandle VKGPUDevice::create( VKGPUDriver * pDriver, const VKGPUDeviceCreateInfo & pCreateInfo )
	{
		return nullptr;
	}

	void VKGPUDevice::initializeCommandSystem()
	{
	}

} // namespace ts3::gpuapi
