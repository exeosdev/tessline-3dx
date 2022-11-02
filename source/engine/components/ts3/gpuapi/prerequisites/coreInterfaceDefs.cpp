
#include "../gpuDriver.h"
#include "../gpuDevice.h"

namespace ts3::GpuAPI
{

	GPUDriverChildObject::GPUDriverChildObject( GPUDriver & pGPUDriver )
	: mGPUDriver( pGPUDriver )
	{}

	GPUDriverChildObject::~GPUDriverChildObject() = default;


	GPUDeviceChildObject::GPUDeviceChildObject( GPUDevice & pGPUDevice )
	: mGPUDriver( pGPUDevice.mGPUDriver )
	, mGPUDevice( pGPUDevice )
	{}

	GPUDeviceChildObject::~GPUDeviceChildObject() = default;

} // namespace ts3::GpuAPI
