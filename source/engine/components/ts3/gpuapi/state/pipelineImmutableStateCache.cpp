
#include "pipelineImmutableStateCache.h"

namespace ts3::gpuapi
{

	PipelineImmutableStateCache2::PipelineImmutableStateCache2( PipelineImmutableStateFactory & pFactory )
	: _stateFactory( &pFactory )
	{}

	PipelineImmutableStateCache2::~PipelineImmutableStateCache2() = default;

	BlendImmutableState * PipelineImmutableStateCache2::getBlendState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	BlendImmutableState * PipelineImmutableStateCache2::getBlendState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	DepthStencilImmutableState * PipelineImmutableStateCache2::getDepthStencilState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	DepthStencilImmutableState * PipelineImmutableStateCache2::getDepthStencilState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	GraphicsShaderLinkageImmutableState * PipelineImmutableStateCache2::getGraphicsShaderLinkageState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	GraphicsShaderLinkageImmutableState * PipelineImmutableStateCache2::getGraphicsShaderLinkageState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	IAInputLayoutImmutableState * PipelineImmutableStateCache2::getIAInputLayoutState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	IAInputLayoutImmutableState * PipelineImmutableStateCache2::getIAInputLayoutState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	RasterizerImmutableState * PipelineImmutableStateCache2::getRasterizerState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	RasterizerImmutableState * PipelineImmutableStateCache2::getRasterizerState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	BlendImmutableState * PipelineImmutableStateCache2::createBlendState(
			const PipelineImmutableStateCreateInfo<BlendConfig> & pCreateInfo )
	{
		return nullptr;
	}

	DepthStencilImmutableState * PipelineImmutableStateCache2::createDepthStencilState(
			const PipelineImmutableStateCreateInfo<DepthStencilConfig> & pCreateInfo )
	{
		return nullptr;
	}

	GraphicsShaderLinkageImmutableState * PipelineImmutableStateCache2::createShaderLinkageState(
			const PipelineImmutableStateCreateInfo<GraphicsShaderSet> & pCreateInfo )
	{
		return nullptr;
	}

	IAInputLayoutImmutableState * PipelineImmutableStateCache2::createIAInputLayoutState(
			const PipelineImmutableStateCreateInfo<IAInputLayoutDefinition> & pCreateInfo )
	{
		return nullptr;
	}

	IAVertexStreamImmutableState * PipelineImmutableStateCache2::createIAVertexStream(
			const PipelineImmutableStateCreateInfo<IAVertexStreamDefinition> & pCreateInfo )
	{
		return nullptr;
	}

	RasterizerImmutableState * PipelineImmutableStateCache2::createRasterizerState(
			const PipelineImmutableStateCreateInfo<RasterizerConfig> & pCreateInfo )
	{
		return nullptr;
	}

	RenderTargetLayoutImmutableState * PipelineImmutableStateCache2::createRenderTargetLayoutState(
			const PipelineImmutableStateCreateInfo<RenderTargetLayoutConfiguration> & pCreateInfo )
	{
		return nullptr;
	}

	RenderTargetBindingImmutableState * PipelineImmutableStateCache2::createRenderTargetBindingState(
			const PipelineImmutableStateCreateInfo<RenderTargetBindingDefinition> & pCreateInfo )
	{
		return nullptr;
	}

	RenderPassImmutableState * PipelineImmutableStateCache2::createRenderPassState(
			const PipelineImmutableStateCreateInfo<RenderPassConfiguration> & pCreateInfo )
	{
		return nullptr;
	}

} // namespace ts3::gpuapi
