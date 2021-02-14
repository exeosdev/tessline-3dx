
#include <ts3/gpuapi/displayManager.h>
#include <ts3/gpuapi/gpuDriver.h>

namespace ts3::gpuapi
{

	GPUDriver::GPUDriver( SysContextHandle pSysContext ) noexcept
	: mSysContext( pSysContext )
	{}

	GPUDriver::~GPUDriver() noexcept = default;

	EGPUDriverID GPUDriver::queryGPUDriverID() const
	{
		return EGPUDriverID::GDID0;
	}

	void GPUDriver::setConfigFlags( Bitmask<GPUDriverConfigFlags> pConfigFlags )
	{
		_configFlags = pConfigFlags;
	}

}
