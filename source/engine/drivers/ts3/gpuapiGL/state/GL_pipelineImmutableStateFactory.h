
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3::gpuapi
{

	struct RenderTargetLayout;

	class GLPipelineImmutableStateFactory : public PipelineImmutableStateFactory
	{
	public:
		GLGPUDevice & mGLGPUDevice;

	public:
		GLPipelineImmutableStateFactory( GLGPUDevice & pGPUDevice );
		virtual ~GLPipelineImmutableStateFactory();

		virtual BlendImmutableStateHandle createBlendState( const BlendConfig & pConfig ) override final;
		virtual DepthStencilImmutableStateHandle createDepthStencilState( const DepthStencilConfig & pConfig ) override final;
		virtual IAVertexStreamImmutableStateHandle createIAVertexStreamState( const IAVertexStreamDefinition & pDefinition ) override final;
		virtual RasterizerImmutableStateHandle createRasterizerState( const RasterizerConfig & pConfig ) override final;
		virtual RenderTargetBindingImmutableStateHandle createRenderTargetBindingState( const RenderTargetBindingDefinition & pDefinition ) override final;
		virtual RenderPassConfigurationImmutableStateHandle createRenderPassState( const RenderPassConfiguration & pConfiguration ) override final;
	};

	class GLPipelineImmutableStateFactoryCore : public GLPipelineImmutableStateFactory
	{
	public:
		GLPipelineImmutableStateFactoryCore( GLGPUDevice & pGPUDevice );
		virtual ~GLPipelineImmutableStateFactoryCore();

		virtual GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet ) override final;
		virtual IAInputLayoutImmutableStateHandle createIAInputLayoutState( const IAInputLayoutDefinition & pDefinition, Shader * pVertexShaderWithBinary ) override final;
	};

	class GLPipelineImmutableStateFactoryCompat : public GLPipelineImmutableStateFactory
	{
	public:
		GLPipelineImmutableStateFactoryCompat( GLGPUDevice & pGPUDevice );
		virtual ~GLPipelineImmutableStateFactoryCompat();

		virtual GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet ) override final;
		virtual IAInputLayoutImmutableStateHandle createIAInputLayoutState( const IAInputLayoutDefinition & pDefinition, Shader * pVertexShaderWithBinary ) override final;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_IMMUTABLE_STATE_FACTORY_H__
