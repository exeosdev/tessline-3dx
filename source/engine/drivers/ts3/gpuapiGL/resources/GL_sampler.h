
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_H__

#include "../objects/GL_samplerObject.h"
#include <ts3/gpuapi/state/sampler.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GLSampler );

	class TS3_GPUAPI_CLASS GLSampler : public Sampler
	{
	public:
		GLSamplerObjectHandle const mGLSamplerObject;

	public:
		GLSampler( GLGPUDevice & pGPUDevice, GLSamplerObjectHandle pGLSamplerObject );
		virtual ~GLSampler();

		static GLSamplerHandle createSampler( GLGPUDevice & pGPUDevice, const SamplerCreateInfo & pCreateInfo );

	private:
		static bool translateSamplerConfig( const SamplerConfig & pSamplerConfig, GLSamplerState & pOutSamplerState );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_H__
