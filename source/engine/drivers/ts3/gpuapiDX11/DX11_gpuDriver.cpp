
#include "DX11_gpuDriver.h"
#include "DX11_gpuDevice.h"

namespace ts3::gpuapi
{

	DX11GPUDriver::DX11GPUDriver( SysContextHandle pSysContext ) noexcept
	: DXGPUDriver( pSysContext )
	{}

	DX11GPUDriver::~DX11GPUDriver() noexcept = default;

	DX11GPUDriverHandle DX11GPUDriver::create( const DX11GPUDriverCreateInfo & pCreateInfo )
	{
		constexpr auto dx11SupportedGPUConfigFlags =
			GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT |
			GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT |
			GPU_DRIVER_CONFIG_FLAG_DISABLE_MULTI_THREAD_ACCESS_BIT |
			GPU_DRIVER_CONFIG_FLAG_USE_REFERENCE_DRIVER_BIT;

		auto dx11GPUDriver = createGPUAPIObject<DX11GPUDriver>( pCreateInfo.sysContext );
		dx11GPUDriver->setConfigFlags( pCreateInfo.configFlags & dx11SupportedGPUConfigFlags );
		return dx11GPUDriver;
	}

	DisplayManagerHandle DX11GPUDriver::createDefaultDisplayManager()
	{
		return nullptr;
	}

	GPUDeviceHandle DX11GPUDriver::createDevice( const GPUDeviceCreateInfo & pCreateInfo )
	{
		DX11GPUDeviceCreateInfo createInfo;
		createInfo.adapterID = pCreateInfo.adapterID;
		createInfo.flags = pCreateInfo.flags;
		return DX11GPUDevice::create( *this, createInfo );
	}

	EGPUDriverID DX11GPUDriver::queryGPUDriverID() const
	{
		return EGPUDriverID::GDIDDX11;
	}

}
