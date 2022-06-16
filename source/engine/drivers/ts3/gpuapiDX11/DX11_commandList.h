
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_COMMAND_LIST_H__
#define __TS3DRIVER_GPUAPI_DX11_COMMAND_LIST_H__

#include "DX11_prerequisites.h"
#include "state/DX11_pipelineStateController.h"
#include <ts3/gpuapi/commandList.h>

namespace ts3::gpuapi
{

	/// @brief
	class TS3_GPUAPI_CLASS DX11CommandList : public CommandList
	{
		friend class DX11CommandSystem;

	public:
		ComPtr<ID3D11Device1> const mD3D11Device1 = nullptr;
		ComPtr<ID3D11DeviceContext1> const mD3D11DeviceContext1 = nullptr;

		DX11CommandList( DX11CommandSystem & pDX11CommandSystem, ECommandListType pListType, ComPtr<ID3D11DeviceContext1> pD3D11DeviceContext1 );
		virtual ~DX11CommandList();

		virtual void beginCommandSequence() override;
		virtual void endCommandSequence() override;

		virtual void executeDeferredContext( CommandContextDeferred & pDeferredContext ) override;

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) override;
		virtual bool setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO ) override;
		virtual bool setRenderTargetStateObject( const RenderTargetStateObject & pRenderTargetSO ) override;

		virtual void clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask ) override;
		virtual void setViewport( const ViewportDesc & pViewportDesc ) override;
		virtual bool setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData ) override;
		virtual bool setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer ) override;
		virtual bool setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture ) override;
		virtual bool setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler ) override;
		virtual void drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset ) override;
		virtual void drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset ) override;
		virtual void drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset ) override;
		virtual void drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset ) override;

	friendapi:
		ID3D11Query * releaseExecutionSyncQuery();

	private:
		void updatePipelineState();

		static ComPtr<ID3D11Query> _createExecutionSyncQuery( ComPtr<ID3D11Device1> pD3D11Device );

	private:
		ComPtr<ID3D11Query> _d3d11ExecutionSyncQuery;
		DX11GraphicsPipelineStateController _stateController;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_COMMAND_LIST_H__
