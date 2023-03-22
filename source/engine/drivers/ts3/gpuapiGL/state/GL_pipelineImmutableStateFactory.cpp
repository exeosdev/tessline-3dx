
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
		return createRenderPassStateDefault( mGLGPUDevice, pConfiguration );
	}

	RenderTargetBindingImmutableStateHandle GLPipelineImmutableStateFactory::createScreenRenderTargetBindingState(
		const RenderTargetLayout & pRenderTargetLayout )
	{
		return GLRenderTargetBindingImmutableState::createForScreen( mGLGPUDevice, pRenderTargetLayout );
	}


	GLPipelineImmutableStateFactoryCore::GLPipelineImmutableStateFactoryCore( GLGPUDevice & pGPUDevice )
	: GLPipelineImmutableStateFactory( pGPUDevice )
	{}

	GLPipelineImmutableStateFactoryCore::~GLPipelineImmutableStateFactoryCore() = default;

	GraphicsShaderLinkageImmutableStateHandle GLPipelineImmutableStateFactoryCore::createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet )
	{
		return GLGraphicsShaderLinkageImmutableStateCore::createInstance( mGLGPUDevice, pShaderSet );
	}

	IAInputLayoutImmutableStateHandle GLPipelineImmutableStateFactoryCore::createIAInputLayoutState(
			const IAInputLayoutDefinition & pDefinition,
			Shader * pVertexShaderWithBinary )
	{
		return GLIAInputLayoutImmutableStateCore::createInstance( mGLGPUDevice, pDefinition );
	}


	GLPipelineImmutableStateFactoryCompat::GLPipelineImmutableStateFactoryCompat( GLGPUDevice & pGPUDevice )
	: GLPipelineImmutableStateFactory( pGPUDevice )
	{}

	GLPipelineImmutableStateFactoryCompat::~GLPipelineImmutableStateFactoryCompat() = default;

	GraphicsShaderLinkageImmutableStateHandle GLPipelineImmutableStateFactoryCompat::createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet )
	{
		return GLGraphicsShaderLinkageImmutableStateCompat::createInstance( mGLGPUDevice, pShaderSet );
	}

	IAInputLayoutImmutableStateHandle GLPipelineImmutableStateFactoryCompat::createIAInputLayoutState(
			const IAInputLayoutDefinition & pDefinition,
			Shader * pVertexShaderWithBinary )
	{
		return GLIAInputLayoutImmutableStateCompat::createInstance( mGLGPUDevice, pDefinition );
	}

}
