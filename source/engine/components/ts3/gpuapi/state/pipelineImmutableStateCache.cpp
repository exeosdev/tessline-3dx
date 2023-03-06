
#include "pipelineImmutableStateCache.h"

namespace ts3::gpuapi
{

	PipelineImmutableStateCache::PipelineImmutableStateCache( PipelineImmutableStateFactory & pFactory );

	PipelineImmutableStateCache::~PipelineImmutableStateCache() = default;

	BlendImmutableState * PipelineImmutableStateCache::getBlendState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	BlendImmutableState * PipelineImmutableStateCache::getBlendState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	DepthStencilImmutableState * PipelineImmutableStateCache::getDepthStencilState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	DepthStencilImmutableState * PipelineImmutableStateCache::getDepthStencilState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	GraphicsShaderLinkageImmutableState * PipelineImmutableStateCache::getGraphicsShaderLinkageState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	GraphicsShaderLinkageImmutableState * PipelineImmutableStateCache::getGraphicsShaderLinkageState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	IAInputLayoutImmutableState * PipelineImmutableStateCache::getIAInputLayoutState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	IAInputLayoutImmutableState * PipelineImmutableStateCache::getIAInputLayoutState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	RasterizerImmutableState * PipelineImmutableStateCache::getRasterizerState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	RasterizerImmutableState * PipelineImmutableStateCache::getRasterizerState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	BlendImmutableState * PipelineImmutableStateCache::createBlendState(
			const PipelineImmutableStateCreateInfo<BlendConfig> & pCreateInfo )
	{
		return nullptr;
	}

	DepthStencilImmutableState * PipelineImmutableStateCache::createDepthStencilState(
			const PipelineImmutableStateCreateInfo<DepthStencilConfig> & pCreateInfo )
	{
		return nullptr;
	}

	GraphicsShaderLinkageImmutableState * PipelineImmutableStateCache::createShaderLinkageState(
			const PipelineImmutableStateCreateInfo<GraphicsShaderSet> & pCreateInfo )
	{
		return nullptr;
	}

	IAInputLayoutImmutableState * PipelineImmutableStateCache::createIAInputLayoutState(
			const PipelineImmutableStateCreateInfo<IAInputLayoutDefinition> & pCreateInfo )
	{
		return nullptr;
	}

	RasterizerImmutableState * PipelineImmutableStateCache::createRasterizerState(
			const PipelineImmutableStateCreateInfo<RasterizerConfig> & pCreateInfo )
	{
		return nullptr;
	}

} // namespace ts3::gpuapi
