
#include "DX12_gpuDriver.h"
#include "DX12_gpuDevice.h"
#include <ts3/system/displayDriverNative.h>

namespace ts3::gpuapi
{

	DX12GPUDriver::DX12GPUDriver( ts3::SysContext * pExfSysContext )
	: DXGPUDriver( pExfSysContext )
	{ }

	DX12GPUDriver::~DX12GPUDriver()
	{ }

	DX12GPUDriverHandle DX12GPUDriver::create( const DX12GPUDriverCreateInfo & pCreateInfo )
	{
		constexpr auto dx12SupportedGPUConfigFlags =
			GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT |
			GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT |
			GPU_DRIVER_CONFIG_FLAG_DISABLE_MULTI_THREAD_ACCESS_BIT |
			GPU_DRIVER_CONFIG_FLAG_USE_REFERENCE_DRIVER_BIT;

		auto dx12GPUDriver = createGPUAPIObject<DX12GPUDriver>( pCreateInfo.exfSysContext );
		dx12GPUDriver->setConfigFlags( pCreateInfo.configFlags & dx12SupportedGPUConfigFlags );

		return dx12GPUDriver;
	}

	DisplayManagerHandle DX12GPUDriver::createDefaultDisplayManager()
	{
		auto * exfDisplayManager = mExfSysContext->getDisplayManager();

		ts3::SysDisplayDriverCreateInfoDXGI dxgiDriverCreateInfo;

		return nullptr;
	}

	GPUDeviceHandle DX12GPUDriver::createDevice( const GPUDeviceCreateInfo & pCreateInfo )
	{
		DX12GPUDeviceCreateInfo createInfo;
		createInfo.adapterID = pCreateInfo.adapterID;
		createInfo.flags = pCreateInfo.flags;
		return DX12GPUDevice::create( *this, createInfo );
	}

}
