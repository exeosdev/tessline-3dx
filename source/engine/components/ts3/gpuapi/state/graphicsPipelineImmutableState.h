
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__

#include "commonGraphicsConfig.h"
#include "inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	enum class EGraphicsPipelineImmutableStateType : uint16
	{
		Blend,
		DepthStencil,
		ShaderLinkage,
		IAInputLayout,
		IAVertexStream,
		Rasterizer,
		RenderTargetBinding,
		RenderPass,
		Unknown
	};

	class GraphicsPipelineImmutableState : public GPUDeviceChildObject
	{
	public:
		GraphicsPipelineImmutableState( const GraphicsPipelineImmutableState & ) = delete;
		GraphicsPipelineImmutableState & operator=( const GraphicsPipelineImmutableState & ) = delete;

		GraphicsPipelineImmutableState( GPUDevice & pGPUDevice );
		virtual ~GraphicsPipelineImmutableState();

		TS3_ATTR_NO_DISCARD virtual EGraphicsPipelineImmutableStateType queryStateType() const noexcept;
	};

	/// @brief Dummy trait type for the descriptor cache. List all the things required for the traits to provide.
	class PipelineImmutableStateFactory
	{
	public:
		virtual ~PipelineImmutableStateFactory() = default;

		virtual BlendImmutableStateHandle createBlendState( const BlendConfig & pConfig ) = 0;
		virtual DepthStencilImmutableStateHandle createDepthStencilState( const DepthStencilConfig & pConfig ) = 0;
		virtual GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet ) = 0;
		virtual IAInputLayoutImmutableStateHandle createIAInputLayoutState( const IAInputLayoutDefinition & pDefinition ) = 0;
		virtual IAVertexStreamImmutableStateHandle createIAVertexStreamState( const IAVertexStreamDefinition & pDefinition ) = 0;
		virtual RasterizerImmutableStateHandle createRasterizerState( const RasterizerConfig & pConfig ) = 0;
		virtual RenderTargetBindingImmutableStateHandle createRenderTargetBindingState( const RenderTargetBindingDefinition & pDefinition ) = 0;
		virtual RenderPassImmutableStateHandle createRenderPassState( const RenderPassConfiguration & pConfiguration ) = 0;
	};

	class PipelineImmutableStateFactoryAdapter
	{
	public:
		PipelineImmutableStateFactoryAdapter( PipelineImmutableStateFactory & pFactory );
		~PipelineImmutableStateFactoryAdapter();

		BlendImmutableStateHandle createState( const BlendConfig & pConfig );
		DepthStencilImmutableStateHandle createState( const DepthStencilConfig & pConfig );
		GraphicsShaderLinkageImmutableStateHandle createState( const GraphicsShaderSet & pShaderSet );
		IAInputLayoutImmutableStateHandle createState( const IAInputLayoutDefinition & pDefinition );
		IAVertexStreamImmutableStateHandle createState( const IAVertexStreamDefinition & pDefinition );
		RasterizerImmutableStateHandle createState( const RasterizerConfig & pConfig );
		RenderTargetBindingImmutableStateHandle createState( const RenderTargetBindingDefinition & pDefinition );
		RenderPassImmutableStateHandle createState( const RenderPassConfiguration & pConfiguration );

	private:
		PipelineImmutableStateFactory * _stateFactory;
	};

	/// @brief
	class PipelineImmutableStateFactoryNull : public PipelineImmutableStateFactory
	{
	public:
		virtual ~PipelineImmutableStateFactoryNull() = default;

		virtual BlendImmutableStateHandle createBlendState( const BlendConfig & ) override final;
		virtual DepthStencilImmutableStateHandle createDepthStencilState( const DepthStencilConfig & ) override final;
		virtual GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageState( const GraphicsShaderSet & ) override final;
		virtual IAInputLayoutImmutableStateHandle createIAInputLayoutState( const IAInputLayoutDefinition & ) override final;
		virtual IAVertexStreamImmutableStateHandle createIAVertexStreamState( const IAVertexStreamDefinition & ) override final;
		virtual RasterizerImmutableStateHandle createRasterizerState( const RasterizerConfig & ) override final;
		virtual RenderTargetBindingImmutableStateHandle createRenderTargetBindingState( const RenderTargetBindingDefinition & ) override final;
		virtual RenderPassImmutableStateHandle createRenderPassState( const RenderPassConfiguration & ) override final;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
