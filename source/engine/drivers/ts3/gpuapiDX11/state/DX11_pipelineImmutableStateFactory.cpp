
#include "DX11_pipelineImmutableStateFactory.h"
#include "DX11_commonGraphicsConfig.h"
#include "DX11_graphicsShaderState.h"
#include "DX11_inputAssembler.h"
#include "../DX11_gpuDevice.h"

namespace ts3::gpuapi
{
	
	DX11PipelineImmutableStateFactory::DX11PipelineImmutableStateFactory( DX11GPUDevice & pGPUDevice )
	: PipelineImmutableStateFactory()
	, mDX11GPUDevice( pGPUDevice )
	{}

	DX11PipelineImmutableStateFactory::~DX11PipelineImmutableStateFactory() = default;

	BlendImmutableStateHandle DX11PipelineImmutableStateFactory::createBlendState(
			const BlendConfig & pConfig )
	{
		return DX11BlendImmutableState::createInstance( mDX11GPUDevice, pConfig );
	}

	DepthStencilImmutableStateHandle DX11PipelineImmutableStateFactory::createDepthStencilState(
			const DepthStencilConfig & pConfig )
	{
		return DX11DepthStencilImmutableState::createInstance( mDX11GPUDevice, pConfig );
	}

	IAVertexStreamImmutableStateHandle DX11PipelineImmutableStateFactory::createIAVertexStreamState(
			const IAVertexStreamDefinition & pDefinition )
	{
		return DX11IAVertexStreamImmutableState::createInstance( mDX11GPUDevice, pDefinition );
	}

	RasterizerImmutableStateHandle DX11PipelineImmutableStateFactory::createRasterizerState(
			const RasterizerConfig & pConfig )
	{
		return DX11RasterizerImmutableState::createInstance( mDX11GPUDevice, pConfig );
	}

	RenderTargetBindingImmutableStateHandle DX11PipelineImmutableStateFactory::createRenderTargetBindingState(
			const RenderTargetBindingDefinition & pDefinition )
	{
		return nullptr;//DX11RenderTargetBindingImmutableState::createInstance( mDX11GPUDevice, pDefinition );
	}

	RenderPassConfigurationImmutableStateHandle DX11PipelineImmutableStateFactory::createRenderPassState(
			const RenderPassConfiguration & pConfiguration )
	{
		return nullptr;//DX11RenderPassConfigurationImmutableState::createInstance( mDX11GPUDevice, pConfiguration );
	}

	RenderTargetBindingImmutableStateHandle DX11PipelineImmutableStateFactory::createScreenRenderTargetBindingState(
			ComPtr<IDXGISwapChain1> pDXGISwapChain )
	{
		return nullptr;//DX11RenderTargetBindingImmutableState::createForScreen( mDX11GPUDevice, pRenderTargetLayout );
	}
	
}
