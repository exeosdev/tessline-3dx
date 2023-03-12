
#pragma once

#ifndef __TS3_GPUAPI_COMMAND_LIST_H__
#define __TS3_GPUAPI_COMMAND_LIST_H__

#include "commonCommandDefs.h"
#include "resources/gpuBufferCommon.h"
#include "resources/samplerCommon.h"

namespace ts3::gpuapi
{

	class GraphicsPipelineStateController;

	enum ECommandListActionFlags : uint32
	{
		E_COMMAND_LIST_ACTION_FLAG_BRP_APPLY_PIPELINE_STATE_BIT = 0x01
	};

	class TS3_GPUAPI_CLASS CommandList : public GPUDeviceChildObject
	{
	public:
		CommandSystem * const mCommandSystem = nullptr;
		ECommandListType const mListType = ECommandListType::Undefined;

		CommandList(
			CommandSystem & pCommandSystem,
			ECommandListType pListType,
			GraphicsPipelineStateController & pPipelineStateController );

		virtual ~CommandList();

		TS3_ATTR_NO_DISCARD bool checkContextSupport( ECommandContextType pContextType ) const noexcept;

		TS3_ATTR_NO_DISCARD bool checkFeatureSupport( Bitmask<ECommandListFlags> pListFlags ) const noexcept;

		TS3_ATTR_NO_DISCARD bool isRenderPassActive() const noexcept;

		TS3_ATTR_NO_DISCARD bool hasPendingGraphicsPipelineStateChanges() const noexcept;

		bool acquireList();
		void releaseList();

		bool applyGraphicsPipelineStateChanges();

		virtual void beginCommandSequence();
		virtual void endCommandSequence();

		bool mapBuffer( GPUBuffer & pBuffer, EGPUMemoryMapMode pMapMode );
		bool mapBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode );
		bool unmapBuffer( GPUBuffer & pBuffer );
		bool flushMappedBuffer( GPUBuffer & pBuffer );
		bool flushMappedBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion );
		bool invalidateBuffer( GPUBuffer & pBuffer );
		bool invalidateBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion );
		bool updateBufferDataCopy( GPUBuffer & pBuffer, GPUBuffer & pSourceBuffer, const GPUBufferDataCopyDesc & pCopyDesc );
		bool updateBufferSubDataCopy( GPUBuffer & pBuffer, GPUBuffer & pSourceBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc );
		bool updateBufferDataUpload( GPUBuffer & pBuffer, const GPUBufferDataUploadDesc & pUploadDesc );
		bool updateBufferSubDataUpload( GPUBuffer & pBuffer, const GPUBufferSubDataUploadDesc & pUploadDesc );

		virtual bool beginRenderPass(
				const RenderPassConfigurationImmutableState & pRenderPassState,
				Bitmask<ECommandListActionFlags> pFlags );

		virtual bool beginRenderPass(
				const RenderPassConfigurationDynamicState & pRenderPassState,
				Bitmask<ECommandListActionFlags> pFlags );

		virtual void endRenderPass();

		bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO );
		bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState );
		bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState );
		bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState );
		bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState );

		bool cmdSetBlendConstantColor( const math::RGBAColorR32Norm & pColor );
		bool cmdSetViewport( const ViewportDesc & pViewportDesc );
		bool cmdSetShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData );
		bool cmdSetShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer );
		bool cmdSetShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture );
		bool cmdSetShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler );

		virtual void cmdDispatchCompute( uint32 pThrGroupSizeX, uint32 pThrGroupSizeY, uint32 pThrGroupSizeZ ) {} // = 0;
		virtual void cmdDispatchComputeIndirect( uint32 pIndirectBufferOffset ) {} // = 0;

		virtual void cmdDrawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset ) = 0;
		virtual void cmdDrawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset ) = 0;
		virtual void cmdDrawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset ) = 0;
		virtual void cmdDrawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset ) = 0;

		virtual void cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext ) = 0;

	private:
		std::atomic<bool> _listLockStatus = ATOMIC_VAR_INIT( false );

		Bitmask<uint32> _internalStateMask;

		GraphicsPipelineStateController * _graphicsPipelineStateController = nullptr;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMAND_LIST_H__
