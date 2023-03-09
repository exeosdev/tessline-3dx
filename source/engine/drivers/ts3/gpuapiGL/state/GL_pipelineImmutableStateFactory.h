
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3::gpuapi
{

	class GLPipelineImmutableStateFactory : public PipelineImmutableStateFactory
	{
	public:
		GLGPUDevice & mGLGPUDevice;

	public:
		GLPipelineImmutableStateFactory( GLGPUDevice & pGLGPUDevice );
		virtual ~GLPipelineImmutableStateFactory();

		virtual BlendImmutableStateHandle createBlendState( const BlendConfig & pConfig ) override final;
		virtual DepthStencilImmutableStateHandle createDepthStencilState( const DepthStencilConfig & pConfig ) override final;
		virtual GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet ) override final;
		virtual IAInputLayoutImmutableStateHandle createIAInputLayoutState( const IAInputLayoutDefinition & pDefinition ) override final;
		virtual IAVertexStreamImmutableStateHandle createIAVertexStreamState( const IAVertexStreamDefinition & pDefinition ) override final;
		virtual RasterizerImmutableStateHandle createRasterizerState( const RasterizerConfig & pConfig ) override final;
		virtual RenderTargetBindingImmutableStateHandle createRenderTargetBindingState( const RenderTargetBindingDefinition & pDefinition ) override final;
		virtual RenderPassConfigurationImmutableStateHandle createRenderPassState( const RenderPassConfiguration & pConfiguration ) override final;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__
