
#include <ts3/gpuapi/displayManager.h>
#include <ts3/gpuapi/gpuDriver.h>

namespace ts3::GpuAPI
{

	GPUDriver::GPUDriver( system::SysContextHandle pSysContext ) noexcept
	: mSysContext( pSysContext )
	{}

	GPUDriver::~GPUDriver() noexcept = default;

	EGPUDriverID GPUDriver::queryGPUDriverID() const
	{
		return EGPUDriverID::GDIUnknown;
	}

	void GPUDriver::setConfigFlags( Bitmask<EGPUDriverConfigFlags> pConfigFlags )
	{
		_configFlags = pConfigFlags;
	}

} // namespace ts3::GpuAPI
