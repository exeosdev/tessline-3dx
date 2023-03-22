
#pragma once

#ifndef __TS3_GPUAPI_COMMAND_LIST_H__
#define __TS3_GPUAPI_COMMAND_LIST_H__

#include "commonCommandDefs.h"
#include "resources/gpuBufferCommon.h"
#include "state/samplerCommon.h"
#include "state/renderPassCommon.h"

namespace ts3::gpuapi
{

	class GraphicsPipelineStateController;

	class TS3_GPUAPI_CLASS CommandList : public GPUDeviceChildObject
	{
	public:
		CommandSystem * const mCommandSystem;

		ECommandListType const mListType;

		Bitmask<ECommandObjectPropertyFlags> const mCommandFlags;

		CommandList(
			CommandSystem & pCommandSystem,
			ECommandListType pListType,
			GraphicsPipelineStateController & pPipelineStateController );

		virtual ~CommandList();

		TS3_ATTR_NO_DISCARD bool checkCommandClassSupport( ECommandQueueClass pQueueClass ) const noexcept;

		TS3_ATTR_NO_DISCARD bool checkFeatureSupport( Bitmask<ECommandObjectPropertyFlags> pCommandListFlags ) const noexcept;

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

		void setRenderPassDynamicState( const GraphicsPipelineDynamicState & pDynamicState );

		bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO );
		bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState );
		bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState );
		bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState );
		bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState );

		bool cmdSetViewport( const ViewportDesc & pViewportDesc );
		bool cmdSetShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData );
		bool cmdSetShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer );
		bool cmdSetShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture );
		bool cmdSetShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler );

		virtual void cmdDispatchCompute( uint32 pThrGroupSizeX, uint32 pThrGroupSizeY, uint32 pThrGroupSizeZ ) {} // = 0;
		virtual void cmdDispatchComputeIndirect( uint32 pIndirectBufferOffset ) {} // = 0;

		virtual void cmdDrawDirectIndexed( native_uint pIndicesNum, native_uint pIndicesOffset ) = 0;
		virtual void cmdDrawDirectIndexedInstanced( native_uint pIndicesNumPerInstance, native_uint pInstancesNum, native_uint pIndicesOffset ) = 0;
		virtual void cmdDrawDirectNonIndexed( native_uint pVerticesNum, native_uint pVerticesOffset ) = 0;
		virtual void cmdDrawDirectNonIndexedInstanced( native_uint pVerticesNumPerInstance, native_uint pInstancesNum, native_uint pVerticesOffset ) = 0;

		virtual void cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext ) = 0;

	private:
		bool onBeginRenderPass( Bitmask<ECommandListActionFlags> pFlags );
		void onEndRenderPass();

	protected:
		std::atomic<bool> _listLockStatus = ATOMIC_VAR_INIT( false );

		Bitmask<uint32> _internalStateMask;

		GraphicsPipelineStateController * _graphicsPipelineStateController = nullptr;
	};

	class TS3_GPUAPI_CLASS CommandListRenderPassDefault : public CommandList
	{
	public:
		CommandListRenderPassDefault(
				CommandSystem & pCommandSystem,
		ECommandListType pListType,
				GraphicsPipelineStateController & pPipelineStateController );

		virtual ~CommandListRenderPassDefault();

		TS3_ATTR_NO_DISCARD const RenderPassConfiguration & getRenderPassConfiguration() const noexcept;

		virtual bool beginRenderPass(
				const RenderPassConfigurationImmutableState & pRenderPassState,
				Bitmask<ECommandListActionFlags> pFlags ) override;

		virtual bool beginRenderPass(
				const RenderPassConfigurationDynamicState & pRenderPassState,
				Bitmask<ECommandListActionFlags> pFlags ) override;

		virtual void endRenderPass() override;

	protected:
		virtual void executeRenderPassLoadActions( const RenderPassConfiguration & pRenderPassConfiguration ) = 0;

		virtual void executeRenderPassStoreActions( const RenderPassConfiguration & pRenderPassConfiguration ) = 0;

	private:
		RenderPassConfiguration _currentRenderPassConfiguration;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMAND_LIST_H__
