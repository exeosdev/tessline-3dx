
#pragma once

#ifndef __TS3_GPUAPI_SAMPLER_H__
#define __TS3_GPUAPI_SAMPLER_H__

#include "samplerCommon.h"

namespace ts3::gpuapi
{

	class TS3_GPUAPI_CLASS Sampler : public GPUDeviceChildObject
	{
	public:
		Sampler( GPUDevice & pGPUDevice );
		virtual ~Sampler();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SAMPLER_H__
