
#pragma once

#ifndef __TS3_GPUAPI_COMMAND_LIST_H__
#define __TS3_GPUAPI_COMMAND_LIST_H__

#include "commonCommandDefs.h"
#include "resources/gpuBufferCommon.h"
#include "resources/samplerCommon.h"

namespace ts3::gpuapi
{

	class GraphicsPipelineStateController;

	class TS3_GPUAPI_CLASS CommandList : public GPUDeviceChildObject
	{
	public:
		CommandSystem * const mCommandSystem = nullptr;
		ECommandListType const mListType;

		CommandList( CommandSystem & pCommandSystem, ECommandListType pListType );
		virtual ~CommandList();

		bool acquireList();
		void releaseList();

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

		virtual bool beginRenderPass( const RenderPassImmutableState & pRenderPassState ) = 0;
		virtual bool beginRenderPass( const RenderPassDynamicState & pRenderPassState ) = 0;

		virtual void endRenderPass() = 0;

		virtual void executeDeferredContext( CommandContextDeferred & pDeferredContext ) = 0;

		virtual void dispatchCompute( uint32 pThrGroupSizeX, uint32 pThrGroupSizeY, uint32 pThrGroupSizeZ ) {} // = 0;
		virtual void dispatchComputeIndirect( uint32 pIndirectBufferOffset ) {} // = 0;

		bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO );
		bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState );
		bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState );
		bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState );
		bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState );

		virtual void setViewport( const ViewportDesc & pViewportDesc ) = 0;
		virtual bool setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData ) = 0;
		virtual bool setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer ) = 0;
		virtual bool setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture ) = 0;
		virtual bool setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler ) = 0;

		virtual void drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset ) = 0;
		virtual void drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset ) = 0;
		virtual void drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset ) = 0;
		virtual void drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset ) = 0;

		bool checkContextSupport( ECommandContextType pContextType ) const;
		bool checkFeatureSupport( Bitmask<ECommandListFlags> pListFlags ) const;

	protected:
		void setGraphicsPipelineStateController( GraphicsPipelineStateController & pStateController );

	private:
		std::atomic<bool> _listLockStatus = ATOMIC_VAR_INIT( false );

		GraphicsPipelineStateController * _pipelineStateController = nullptr;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMAND_LIST_H__
