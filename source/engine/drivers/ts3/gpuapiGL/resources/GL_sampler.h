
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_H__

#include "../objects/GL_samplerObject.h"
#include <ts3/gpuapi/resources/sampler.h>

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( GLSampler );

	class TS3_GPUAPI_CLASS GLSampler : public Sampler
	{
	public:
		GLSamplerObjectHandle const mGLSamplerObject;

	public:
		GLSampler( GLGPUDevice & pGPUDevice, const SamplerDesc & pSamplerDesc, GLSamplerObjectHandle pGLSamplerObject );
		virtual ~GLSampler();

		static GLSamplerHandle createSampler( GLGPUDevice & pGLGPUDevice, const SamplerCreateInfo & pCreateInfo );

	private:
		static bool translateSamplerDesc( const SamplerDesc & pSamplerDesc, GLSamplerState & pOutSamplerState );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_H__
