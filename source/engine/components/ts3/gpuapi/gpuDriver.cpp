
#include <ts3/gpuapi/displayManager.h>
#include <ts3/gpuapi/gpuDriver.h>
#include <ts3/gpuapi/gpuDevice.h>

namespace ts3::gpuapi
{

	GPUDriver::GPUDriver( system::SysContextHandle pSysContext ) noexcept
	: mSysContext( pSysContext )
	{}

	GPUDriver::~GPUDriver() noexcept = default;

	EGPUDriverID GPUDriver::queryGPUDriverID() const
	{
		return EGPUDriverID::GDIUnknown;
	}

	DisplayManagerHandle GPUDriver::createDefaultDisplayManager()
	{
		return nullptr;
	}

	GPUDeviceHandle GPUDriver::createDevice( const GPUDeviceCreateInfo & pCreateInfo )
	{
		auto gpuDevice = _drvCreateDevice( pCreateInfo );
		if( !gpuDevice )
		{
			return nullptr;
		}

		gpuDevice->initializeCommandSystem();

		return gpuDevice;
	}

	void GPUDriver::setConfigFlags( Bitmask<EGPUDriverConfigFlags> pConfigFlags )
	{
		_configFlags = pConfigFlags;
	}

} // namespace ts3::gpuapi
