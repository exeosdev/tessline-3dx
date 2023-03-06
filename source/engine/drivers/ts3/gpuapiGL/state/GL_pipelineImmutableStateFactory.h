
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3::gpuapi
{

	class GLPipelineImmutableStateFactory : public PipelineImmutableStateFactory
	{
	public:
		GLPipelineImmutableStateFactory( GPUDevice & pGPUDevice )
		: PipelineImmutableStateFactory( pGPUDevice )
		{}

		virtual ~GLPipelineImmutableStateFactory() = default;

		virtual BlendImmutableStateHandle createBlendState( const BlendConfig & pConfig ) override;

		virtual DepthStencilImmutableStateHandle createDepthStencilState( const DepthStencilConfig & pConfig ) override;

		virtual IAInputLayoutImmutableStateHandle createIAInputLayoutState( const IAInputLayoutDefinition & pDefinition ) override;

		virtual RasterizerImmutableStateHandle createRasterizerState( const RasterizerConfig & pConfig ) override;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__
