
#include "sampler.h"

namespace ts3::gpuapi
{

	Sampler::Sampler( GPUDevice & pGPUDevice, const SamplerDesc & pSamplerDesc )
	: GPUBaseObject( pGPUDevice )
	, mSamplerDesc( pSamplerDesc )
	{}

	Sampler::~Sampler() = default;

}
