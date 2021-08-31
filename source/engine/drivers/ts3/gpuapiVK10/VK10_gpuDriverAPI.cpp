
#include "VK10_gpuDriverAPI.h"
#include "ts3/gpuapiVK/VK_gpuDriver.h"
#include "ts3/gpuapiVK/VK_gpuDevice.h"
#include <ts3/gpuapiVK/system/VK_presentationLayer.h>

namespace ts3::gpuapi
{

	GPUDriverHandle VK10GPUDriverInterface::createDriver( const GPUDriverCreateInfo & pCreateInfo )
	{
		VKGPUDriverCreateInfo vk10CreateInfo;
		vk10CreateInfo.exfSysContext = pCreateInfo.exfSysContext;
		vk10CreateInfo.initFlags = pCreateInfo.initFlags;

		return VKGPUDriver::create( vk10CreateInfo );
	}

	PresentationLayerHandle VK10GPUDriverInterface::createScreenPresentationLayer( GPUDevice * pDevice, const PresentationLayerCreateInfo & pCreateInfo )
	{
		VKPresentationLayerCreateInfo vk10CreateInfo;
		vk10CreateInfo.coreEngineState = pCreateInfo.coreEngineState;
		vk10CreateInfo.screenRect = pCreateInfo.screenRect;
		vk10CreateInfo.visualConfig = pCreateInfo.visualConfig;
		vk10CreateInfo.displayConfigFlags = pCreateInfo.displayConfigFlags;

		auto * deviceVK = pDevice->queryInterface<VKGPUDevice>();
		return VKScreenPresentationLayer::create( deviceVK, vk10CreateInfo );
	}

}