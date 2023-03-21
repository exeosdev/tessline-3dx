
#include "DX_gpuDevice.h"
#include "DX_gpuDriver.h"
#include "DX_apiTranslationLayer.h"

namespace ts3::gpuapi
{

	DXGPUDevice::DXGPUDevice( DXGPUDriver & pDXGPUDriver, ComPtr<IDXGIFactory2> pDXGIFactory2 )
	: GPUDevice( pDXGPUDriver )
	, mDXGIFactory2( std::move( pDXGIFactory2 ) )
	, mDXGIDebug( atl::queryDXGIDebugInterface( pDXGPUDriver.getConfigFlags() ) )
	, mDXGIInfoQueue( atl::queryDXGIDebugInfoQueue( pDXGPUDriver.getConfigFlags() ) )
	{}

	DXGPUDevice::~DXGPUDevice() = default;

} // namespace ts3::gpuapi
