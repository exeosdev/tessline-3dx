
#include "graphicsPipelineImmutableState.h"
#include "graphicsShaderLinkageImmutableState.h"

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


	PipelineImmutableStateFactorySeparableShader::PipelineImmutableStateFactorySeparableShader( GPUDevice & pGPUDevice )
	: mGPUDevice( pGPUDevice )
	{}

	PipelineImmutableStateFactorySeparableShader::~PipelineImmutableStateFactorySeparableShader() = default;

	GraphicsShaderLinkageImmutableStateHandle PipelineImmutableStateFactorySeparableShader::createGraphicsShaderLinkageState(
			const GraphicsShaderSet & pShaderSet )
	{
		if( !pShaderSet[EShaderType::GSVertex] || !pShaderSet[EShaderType::GSPixel] )
		{
			return nullptr;
		}

		if( !pShaderSet.validateShaders() )
		{
			return nullptr;
		}

		const auto stateCommonProperties = smutil::getGraphicsShaderLinkageCommonPropertiesForShaderSet( pShaderSet );

		const auto shaderImmutableState = createGPUAPIObject<GraphicsShaderImmutableStateSeparable>(
				mGPUDevice,
				std::move( pShaderSet ),
				stateCommonProperties );

		return shaderImmutableState;
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

	RenderTargetBindingImmutableStateHandle PipelineImmutableStateFactoryNull::createRenderTargetBindingState( const RenderTargetBindingDefinition & )
	{
		return nullptr;
	}

	RenderPassConfigurationImmutableStateHandle PipelineImmutableStateFactoryNull::createRenderPassState( const RenderPassConfiguration & )
	{
		return nullptr;
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

	RenderTargetBindingImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const RenderTargetBindingDefinition & pDefinition )
	{
		return _stateFactory->createRenderTargetBindingState( pDefinition );
	}

	RenderPassConfigurationImmutableStateHandle PipelineImmutableStateFactoryAdapter::createState( const RenderPassConfiguration & pConfiguration )
	{
		return _stateFactory->createRenderPassState( pConfiguration );
	}

} // namespace ts3::gpuapi
