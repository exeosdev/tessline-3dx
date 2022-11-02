
#include "gpuStateObject.h"

namespace ts3::GpuAPI
{

	GPUStateObject::GPUStateObject( GPUDevice & pGPUDevice )
	: GPUBaseObject( pGPUDevice )
	{}

	GPUStateObject::~GPUStateObject() = default;

} // namespace ts3::GpuAPI
