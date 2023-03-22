
#include "DX11_gpuDriver.h"
#include "DX11_gpuDevice.h"

namespace ts3::gpuapi
{

	DX11GPUDriver::DX11GPUDriver( system::SysContextHandle pSysContext ) noexcept
	: DXGPUDriver( pSysContext )
	{}

	DX11GPUDriver::~DX11GPUDriver() noexcept = default;

	DX11GPUDriverHandle DX11GPUDriver::create( const DX11GPUDriverCreateInfo & pCreateInfo )
	{
		constexpr auto dx11SupportedGPUConfigFlags =
			E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT |
			E_GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT |
			E_GPU_DRIVER_CONFIG_FLAG_DISABLE_MULTI_THREAD_ACCESS_BIT |
			E_GPU_DRIVER_CONFIG_FLAG_USE_REFERENCE_DRIVER_BIT;

		auto dx11GPUDriver = createGPUAPIObject<DX11GPUDriver>( pCreateInfo.sysContext );
		dx11GPUDriver->setConfigFlags( pCreateInfo.configFlags & dx11SupportedGPUConfigFlags );
		return dx11GPUDriver;
	}

	EGPUDriverID DX11GPUDriver::queryGPUDriverID() const noexcept
	{
		return EGPUDriverID::GDIDirectX11;
	}

	DisplayManagerHandle DX11GPUDriver::_drvCreateDefaultDisplayManager()
	{
		return nullptr;
	}

	GPUDeviceHandle DX11GPUDriver::_drvCreateDevice( const GPUDeviceCreateInfo & pCreateInfo )
	{
		DX11GPUDeviceCreateInfo createInfo;
		createInfo.adapterID = pCreateInfo.adapterID;
		createInfo.flags = pCreateInfo.flags;
		return DX11GPUDevice::create( *this, createInfo );
	}

} // namespace ts3::gpuapi
