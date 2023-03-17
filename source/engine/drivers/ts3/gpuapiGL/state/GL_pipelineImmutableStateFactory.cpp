
#include "GL_pipelineImmutableStateFactory.h"
#include "GL_commonGraphicsConfig.h"
#include "GL_graphicsShaderState.h"
#include "GL_inputAssembler.h"
#include "GL_renderTarget.h"
#include "../GL_gpuDevice.h"

namespace ts3::gpuapi
{

	GLPipelineImmutableStateFactory::GLPipelineImmutableStateFactory( GLGPUDevice & pGPUDevice )
	: PipelineImmutableStateFactory()
	, mGLGPUDevice( pGPUDevice )
	{}

	GLPipelineImmutableStateFactory::~GLPipelineImmutableStateFactory() = default;

	BlendImmutableStateHandle GLPipelineImmutableStateFactory::createBlendState(
			const BlendConfig & pConfig )
	{
		return GLBlendImmutableState::createInstance( mGLGPUDevice, pConfig );
	}

	DepthStencilImmutableStateHandle GLPipelineImmutableStateFactory::createDepthStencilState(
			const DepthStencilConfig & pConfig )
	{
		return GLDepthStencilImmutableState::createInstance( mGLGPUDevice, pConfig );
	}

	IAInputLayoutImmutableStateHandle GLPipelineImmutableStateFactory::createIAInputLayoutState(
			const IAInputLayoutDefinition & pDefinition )
	{
		return GLIAInputLayoutImmutableState::createInstance( mGLGPUDevice, pDefinition );
	}

	IAVertexStreamImmutableStateHandle GLPipelineImmutableStateFactory::createIAVertexStreamState(
			const IAVertexStreamDefinition & pDefinition )
	{
		return GLIAVertexStreamImmutableState::createInstance( mGLGPUDevice, pDefinition );
	}

	RasterizerImmutableStateHandle GLPipelineImmutableStateFactory::createRasterizerState(
			const RasterizerConfig & pConfig )
	{
		return GLRasterizerImmutableState::createInstance( mGLGPUDevice, pConfig );
	}

	RenderTargetBindingImmutableStateHandle GLPipelineImmutableStateFactory::createRenderTargetBindingState(
			const RenderTargetBindingDefinition & pDefinition )
	{
		return GLRenderTargetBindingImmutableState::createInstance( mGLGPUDevice, pDefinition );
	}

	RenderPassConfigurationImmutableStateHandle GLPipelineImmutableStateFactory::createRenderPassState(
			const RenderPassConfiguration & pConfiguration )
	{
		return GLRenderPassConfigurationImmutableState::createInstance( mGLGPUDevice, pConfiguration );
	}


	GLPipelineImmutableStateFactoryCore::GLPipelineImmutableStateFactoryCore( GLGPUDevice & pGPUDevice )
	: GLPipelineImmutableStateFactory( pGPUDevice )
	{}

	GLPipelineImmutableStateFactoryCore::~GLPipelineImmutableStateFactoryCore() = default;

	GraphicsShaderLinkageImmutableStateHandle GLPipelineImmutableStateFactoryCore::createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet )
	{
		return GLGraphicsShaderLinkageImmutableStateCore::createInstance( mGLGPUDevice, pShaderSet );
	}


	GLPipelineImmutableStateFactoryCompat::GLPipelineImmutableStateFactoryCompat( GLGPUDevice & pGPUDevice )
	: GLPipelineImmutableStateFactory( pGPUDevice )
	{}

	GLPipelineImmutableStateFactoryCompat::~GLPipelineImmutableStateFactoryCompat() = default;

	GraphicsShaderLinkageImmutableStateHandle GLPipelineImmutableStateFactoryCompat::createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet )
	{
		return GLGraphicsShaderLinkageImmutableStateCompat::createInstance( mGLGPUDevice, pShaderSet );
	}

}
