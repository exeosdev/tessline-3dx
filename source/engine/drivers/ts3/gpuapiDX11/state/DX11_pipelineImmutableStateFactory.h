
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_PIPELINE_IMMUTABLE_STATE_FACTORY_H__
#define __TS3DRIVER_GPUAPI_DX11_PIPELINE_IMMUTABLE_STATE_FACTORY_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3::gpuapi
{

	struct RenderTargetLayout;

	class DX11PipelineImmutableStateFactory : public PipelineImmutableStateFactorySeparableShader
	{
	public:
		DX11GPUDevice & mDX11GPUDevice;

	public:
		DX11PipelineImmutableStateFactory( DX11GPUDevice & pGPUDevice );
		virtual ~DX11PipelineImmutableStateFactory();

		virtual BlendImmutableStateHandle createBlendState( const BlendConfig & pConfig ) override final;
		virtual DepthStencilImmutableStateHandle createDepthStencilState( const DepthStencilConfig & pConfig ) override final;
		virtual IAInputLayoutImmutableStateHandle createIAInputLayoutState( const IAInputLayoutDefinition & pDefinition, Shader * pVertexShaderWithBinary ) override final;
		virtual IAVertexStreamImmutableStateHandle createIAVertexStreamState( const IAVertexStreamDefinition & pDefinition ) override final;
		virtual RasterizerImmutableStateHandle createRasterizerState( const RasterizerConfig & pConfig ) override final;
		virtual RenderTargetBindingImmutableStateHandle createRenderTargetBindingState( const RenderTargetBindingDefinition & pDefinition ) override final;
		virtual RenderPassConfigurationImmutableStateHandle createRenderPassState( const RenderPassConfiguration & pConfiguration ) override final;

		RenderTargetBindingImmutableStateHandle createScreenRenderTargetBindingState( ComPtr<IDXGISwapChain1> pDXGISwapChain );
	};
	
} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_PIPELINE_IMMUTABLE_STATE_FACTORY_H__
