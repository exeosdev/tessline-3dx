

#include "DX_gpuDriver.h"

namespace ts3::gpuapi
{

	DXGPUDriver::DXGPUDriver( system::SysContextHandle pSysContext ) noexcept
	: GPUDriver( pSysContext )
	{}

	DXGPUDriver::~DXGPUDriver() noexcept = default;

}
