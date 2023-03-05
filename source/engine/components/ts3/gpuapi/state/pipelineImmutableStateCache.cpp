
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

	RasterizerImmutableState * PipelineImmutableStateCache::getRasterizerState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	RasterizerImmutableState * PipelineImmutableStateCache::getRasterizerState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	VertexInputLayoutImmutableState * PipelineImmutableStateCache::getVertexInputLayoutState( const UniqueGPUObjectID & pDescriptorID ) const
	{
		return nullptr;
	}

	VertexInputLayoutImmutableState * PipelineImmutableStateCache::getVertexInputLayoutState( const UniqueGPUObjectName & pDescriptorName ) const
	{
		return nullptr;
	}

	BlendImmutableState * PipelineImmutableStateCache::createBlendState(
			const PipelineImmutableStateCreateInfo<BlendDescriptor> & pCreateInfo )
	{
		return nullptr;
	}

	DepthStencilImmutableState * PipelineImmutableStateCache::createDepthStencilState(
			const PipelineImmutableStateCreateInfo<DepthStencilDescriptor> & pCreateInfo )
	{
		return nullptr;
	}

	GraphicsShaderLinkageImmutableState * PipelineImmutableStateCache::createShaderLinkageState(
			const PipelineImmutableStateCreateInfo<GraphicsShaderLinkageDescriptor> & pCreateInfo )
	{
		return nullptr;
	}

	RasterizerImmutableState * PipelineImmutableStateCache::createRasterizerState(
			const PipelineImmutableStateCreateInfo<RasterizerDescriptor> & pCreateInfo )
	{
		return nullptr;
	}

	VertexInputLayoutImmutableState * PipelineImmutableStateCache::createVertexInputLayoutState(
			const PipelineImmutableStateCreateInfo<VertexInputLayoutDescriptor> & pCreateInfo )
	{
		return nullptr;
	}

} // namespace ts3::gpuapi
