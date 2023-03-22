
#include "sampler.h"

namespace ts3::gpuapi
{

	Sampler::Sampler( GPUDevice & pGPUDevice )
	: GPUDeviceChildObject( pGPUDevice )
	{}

	Sampler::~Sampler() = default;

} // namespace ts3::gpuapi
