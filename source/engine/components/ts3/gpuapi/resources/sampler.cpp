
#include "sampler.h"

namespace ts3
{
namespace gpuapi
{

	Sampler::Sampler( GPUDevice & pGPUDevice, const SamplerDesc & pSamplerDesc )
	: GPUBaseObject( pGPUDevice )
	, mSamplerDesc( pSamplerDesc )
	{}

	Sampler::~Sampler() = default;

} /* namespace ts3 */
} /* namespace gpuapi */
