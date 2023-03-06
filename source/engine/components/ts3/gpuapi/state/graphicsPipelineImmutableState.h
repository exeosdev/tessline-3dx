
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
		Rasterizer,
		ShaderLinkage,
		IAInputLayout,
		IAVertexStream,
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
	class PipelineImmutableStateFactory : public GPUDeviceChildObject
	{
	public:
		PipelineImmutableStateFactory( GPUDevice & pGPUDevice )
		: GPUDeviceChildObject( pGPUDevice )
		{}

		virtual ~PipelineImmutableStateFactory() = default;

		virtual BlendImmutableStateHandle createBlendState( const BlendConfig & pConfig ) = 0;

		virtual DepthStencilImmutableStateHandle createDepthStencilState( const DepthStencilConfig & pConfig ) = 0;

		virtual GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet ) = 0;

		virtual IAInputLayoutImmutableStateHandle createIAInputLayoutState( const IAInputLayoutDefinition & pDefinition ) = 0;

		virtual RasterizerImmutableStateHandle createRasterizerState( const RasterizerConfig & pConfig ) = 0;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_H__
