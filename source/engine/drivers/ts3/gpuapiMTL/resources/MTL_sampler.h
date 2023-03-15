
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_SAMPLER_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_SAMPLER_H__

#include "../MTL_prerequisites.h"
#include <ts3/gpuapi/state/sampler.h>

#include <Metal/MTLSampler.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( MetalSampler );

	class TS3_GPUAPI_CLASS MetalSampler : public Sampler
	{
	public:
		id<MTLSamplerState> const mMTLSamplerState;

	public:
		MetalSampler( MetalGPUDevice & pGPUDevice, id<MTLSamplerState> pMTLSamplerState );
		virtual ~MetalSampler();

		static MetalSamplerHandle createSampler( MetalGPUDevice & pGPUDevice, const SamplerCreateInfo & pCreateInfo );

	private:
		static MTLSamplerDescriptor * translateSamplerConfig( const SamplerConfig & pSamplerConfig );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_SAMPLER_H__
