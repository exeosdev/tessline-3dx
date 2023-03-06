
#include "../gpuDriver.h"
#include "../gpuDevice.h"

namespace ts3::gpuapi
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

	UniqueGPUObjectID GPUDeviceChildObject::queryObjectID() const
	{
		return cxdefs::GPU_OBJECT_ID_EMPTY;
	}

} // namespace ts3::gpuapi
