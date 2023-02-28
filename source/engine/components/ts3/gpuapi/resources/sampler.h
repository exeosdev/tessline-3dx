
#pragma once

#ifndef __TS3_GPUAPI_SAMPLER_H__
#define __TS3_GPUAPI_SAMPLER_H__

#include "samplerCommon.h"

namespace ts3::gpuapi
{

	class TS3_GPUAPI_CLASS Sampler : public GPUDeviceChildObject
	{
	public:
		SamplerDesc const mSamplerDesc;

		Sampler( GPUDevice & pGPUDevice, const SamplerDesc & pSamplerDesc );
		virtual ~Sampler();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SAMPLER_H__
