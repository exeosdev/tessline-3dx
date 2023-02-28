
#pragma once

#ifndef __TS3_GPUAPI_COMMAND_LIST_H__
#define __TS3_GPUAPI_COMMAND_LIST_H__

#include "commonCommandDefs.h"
#include "resources/gpuBufferCommon.h"
#include "resources/samplerCommon.h"

namespace ts3::gpuapi
{

	class TS3_GPUAPI_CLASS CommandList : public GPUDeviceChildObject
	{
	public:
		CommandSystem * const mCommandSystem = nullptr;
		ECommandListType const mListType;

		CommandList( CommandSystem & pCommandSystem, ECommandListType pListType );
		virtual ~CommandList();

		bool acquireList();
		void releaseList();

		void setColorBufferClearValue( const math::RGBAColorR32Norm & pColorClearValue );
		void setDepthBufferClearValue( float pDepthClearValue );
		void setStencilBufferClearValue( uint8 pStencilClearValue );

		virtual void initializeClearState();
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

		virtual bool beginRenderPass();
		virtual void endRenderPass();

		virtual void executeDeferredContext( CommandContextDeferred & pDeferredContext ) = 0;

		virtual void dispatchCompute( uint32 pThrGroupSizeX, uint32 pThrGroupSizeY, uint32 pThrGroupSizeZ ) {} // = 0;
		virtual void dispatchComputeIndirect( uint32 pIndirectBufferOffset ) {} // = 0;

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) = 0;
		virtual bool setRenderTargetStateObject( const RenderTargetStateObject & pRenderTargetSO ) = 0;

		virtual void clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask ) = 0;
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
		const RenderTargetClearConfig & getRenderTargetClearConfig() const;

	private:
		enum class ListStatus : uint32
		{
			Acquired,
			Available
		};

		RenderTargetClearConfig _renderTargetClearConfig;
		std::atomic<ListStatus> _listStatusFlag = ATOMIC_VAR_INIT( ListStatus::Available );
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMAND_LIST_H__
