
#include "graphicsPipelineImmutableState.h"

namespace ts3::gpuapi
{

	GraphicsPipelineImmutableState::GraphicsPipelineImmutableState( GPUDevice & pGPUDevice )
	: GPUDeviceChildObject( pGPUDevice )
	{}

	GraphicsPipelineImmutableState::~GraphicsPipelineImmutableState() = default;

	EGraphicsPipelineImmutableStateType GraphicsPipelineImmutableState::queryStateType() const noexcept
	{
		return EGraphicsPipelineImmutableStateType::Unknown;
	}


	PipelineImmutableStateFactoryAdapter::PipelineImmutableStateFactoryAdapter( PipelineImmutableStateFactory & pFactory )
	: _stateFactory( &pFactory )
	{}

	PipelineImmutableStateFactoryAdapter::~PipelineImmutableStateFactoryAdapter() = default;

	BlendImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const BlendConfig & pConfig )
	{
		return _stateFactory->createBlendState( pConfig );
	}

	DepthStencilImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const DepthStencilConfig & pConfig )
	{
		return _stateFactory->createDepthStencilState( pConfig );
	}

	GraphicsShaderLinkageImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const GraphicsShaderSet & pShaderSet )
	{
		return _stateFactory->createGraphicsShaderLinkageState( pShaderSet );
	}

	IAInputLayoutImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const IAInputLayoutDefinition & pDefinition )
	{
		return _stateFactory->createIAInputLayoutState( pDefinition );
	}

	IAVertexStreamImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const IAVertexStreamDefinition & pDefinition )
	{
		return _stateFactory->createIAVertexStreamState( pDefinition );
	}

	RasterizerImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const RasterizerConfig & pConfig )
	{
		return _stateFactory->createRasterizerState( pConfig );
	}

	RenderTargetLayoutImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const RenderTargetLayoutConfiguration & pConfiguration )
	{
		return _stateFactory->createRenderTargetLayoutState( pConfiguration );
	}

	RenderTargetBindingImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const RenderTargetBindingDefinition & pDefinition )
	{
		return _stateFactory->createRenderTargetBindingState( pDefinition );
	}

	RenderPassImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const RenderPassConfiguration & pConfiguration )
	{
		return _stateFactory->createRenderPassState( pConfiguration );
	}


	BlendImmutableStateHandle PipelineImmutableStateFactoryNull::createBlendState( const BlendConfig & )
	{
		return nullptr;
	}

	DepthStencilImmutableStateHandle PipelineImmutableStateFactoryNull::createDepthStencilState( const DepthStencilConfig & )
	{
		return nullptr;
	}

	GraphicsShaderLinkageImmutableStateHandle PipelineImmutableStateFactoryNull::createGraphicsShaderLinkageState( const GraphicsShaderSet & )
	{
		return nullptr;
	}

	IAInputLayoutImmutableStateHandle PipelineImmutableStateFactoryNull::createIAInputLayoutState( const IAInputLayoutDefinition & )
	{
		return nullptr;
	}

	IAVertexStreamImmutableStateHandle PipelineImmutableStateFactoryNull::createIAVertexStreamState( const IAVertexStreamDefinition & )
	{
		return nullptr;
	}

	RasterizerImmutableStateHandle PipelineImmutableStateFactoryNull::createRasterizerState( const RasterizerConfig & )
	{
		return nullptr;
	}

	RenderTargetLayoutImmutableStateHandle PipelineImmutableStateFactoryNull::createRenderTargetLayoutState( const RenderTargetLayoutConfiguration & )
	{
		return nullptr;
	}

	RenderTargetBindingImmutableStateHandle PipelineImmutableStateFactoryNull::createRenderTargetBindingState( const RenderTargetBindingDefinition & )
	{
		return nullptr;
	}

	RenderPassImmutableStateHandle PipelineImmutableStateFactoryNull::createRenderPassState( const RenderPassConfiguration & )
	{
		return nullptr;
	}

} // namespace ts3::gpuapi
