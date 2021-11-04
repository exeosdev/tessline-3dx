

#include "DX_gpuDriver.h"

namespace ts3
{
namespace gpuapi
{

	DXGPUDriver::DXGPUDriver( system::SysContextHandle pSysContext ) noexcept
	: GPUDriver( pSysContext )
	{}

	DXGPUDriver::~DXGPUDriver() noexcept = default;

} /* namespace ts3 */
} /* namespace gpuapi */
