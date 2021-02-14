

#include "DX_gpuDriver.h"

namespace ts3::gpuapi
{

	DXGPUDriver::DXGPUDriver( SysContextHandle pSysContext ) noexcept
	: GPUDriver( pSysContext )
	{}

	DXGPUDriver::~DXGPUDriver() noexcept = default;

}
