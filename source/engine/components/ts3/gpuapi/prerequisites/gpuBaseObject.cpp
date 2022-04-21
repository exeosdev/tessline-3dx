
#include "../gpuDevice.h"

namespace ts3::gpuapi
{

	GPUBaseObject::GPUBaseObject( GPUDevice & pGPUDevice )
	: GPUDeviceChildObject( pGPUDevice )
	{}

	GPUBaseObject::~GPUBaseObject() = default;

} // namespace ts3::gpuapi
