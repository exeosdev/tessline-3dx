
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_PIPELINE_STATE_CONTROLLER_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_PIPELINE_STATE_CONTROLLER_H__

#include "../MTL_prerequisites.h"
#include <ts3/gpuapi/state/separablePipelineState.h>

namespace ts3::gpuapi
{

	/// @brief
	class MTLGraphicsPipelineStateController : public GraphicsPipelineStateController
	{
	public:
		MetalCommandList * const mMetalCommandList;

	public:
		MTLGraphicsPipelineStateController( MetalCommandList & pMetalCommandList );
		~MTLGraphicsPipelineStateController();

		virtual bool applyStateChanges() override final;

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO ) override;
		virtual bool resetGraphicsPipelineStateObject() override;

		virtual bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState ) override;
		virtual bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState ) override;
		virtual bool resetIAVertexStreamState() override;

		virtual bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState ) override;
		virtual bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState ) override;
		virtual bool resetRenderTargetBindingState() override;

		virtual bool setViewport( const ViewportDesc & pViewportDesc ) override;
		virtual bool setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData ) override;
		virtual bool setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer ) override;
		virtual bool setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture ) override;
		virtual bool setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler ) override;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_PIPELINE_STATE_CONTROLLER_H__
