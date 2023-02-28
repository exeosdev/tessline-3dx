
#include "sampler.h"

namespace ts3::gpuapi
{

	Sampler::Sampler( GPUDevice & pGPUDevice, const SamplerDesc & pSamplerDesc )
	: GPUDeviceChildObject( pGPUDevice )
	, mSamplerDesc( pSamplerDesc )
	{}

	Sampler::~Sampler() = default;

} // namespace ts3::gpuapi
