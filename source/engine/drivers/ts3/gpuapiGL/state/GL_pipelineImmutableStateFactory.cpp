
#include "GL_pipelineImmutableStateFactory.h"
#include "GL_graphicsPipelineImmutableState.h"

namespace ts3::gpuapi
{

	BlendImmutableStateHandle GLPipelineImmutableStateFactory::createBlendState( const BlendConfig & pConfig )
	{
		const auto openglConfig = smutil::translateBlendConfig( pConfig );
		auto immutableState = createGPUAPIObject<GLBlendImmutableState>( mGPUDevice, openglConfig );
		return immutableState;
	}

	DepthStencilImmutableStateHandle GLPipelineImmutableStateFactory::createDepthStencilState( const DepthStencilConfig & pConfig )
	{
		const auto openglConfig = smutil::translateDepthStencilConfig( pConfig );
		auto immutableState = createGPUAPIObject<GLDepthStencilImmutableState>( mGPUDevice, openglConfig );
		return immutableState;
	}

	IAInputLayoutImmutableStateHandle GLPipelineImmutableStateFactory::createIAInputLayoutState( const IAInputLayoutDefinition & pDefinition )
	{
		const auto openglDefinition = smutil::translateIAInputLayoutDefinition( pDefinition );
		auto immutableState = createGPUAPIObject<GLIAInputLayoutImmutableState>( mGPUDevice, openglDefinition );
		return immutableState;
	}

	RasterizerImmutableStateHandle GLPipelineImmutableStateFactory::createRasterizerState( const RasterizerConfig & pConfig )
	{
		const auto openglConfig = smutil::translateRasterizerConfig( pConfig );
		auto immutableState = createGPUAPIObject<GLRasterizerImmutableState>( mGPUDevice, openglConfig );
		return immutableState;
	}

}
