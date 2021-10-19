
#include "commandContext.h"
#include "commandList.h"
#include "commandSystem.h"
#include "gpuDevice.h"

namespace ts3::gpuapi
{

	const Bitmask<ECommandListFlags> CommandContext::sListFlagsCommon = E_COMMAND_LIST_FLAG_COMMAND_CLASS_COMMON_BIT;

	CommandContext::CommandContext( CommandSystem & pCommandSystem, CommandList & pCmdList )
	: GPUDeviceChildObject( pCommandSystem.mGPUDevice )
	, mCommandList( &pCmdList )
	, mCommandSystem( mCommandList->mCommandSystem )
	{}

	CommandContext::~CommandContext() = default;

	void CommandContext::beginCommandSequence()
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsCommon ) );
		return mCommandList->beginCommandSequence();
	}

	void CommandContext::endCommandSequence()
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsCommon ) );
		return mCommandList->endCommandSequence();
	}

	bool CommandContext::mapBuffer( GPUBuffer & pBuffer, EGPUMemoryMapMode pMapMode )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsCommon ) );
		return mCommandList->mapBuffer( pBuffer, pMapMode );
	}

	bool CommandContext::mapBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsCommon ) );
		return mCommandList->mapBufferRegion( pBuffer, pRegion, pMapMode );
	}

	bool CommandContext::unmapBuffer( GPUBuffer & pBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsCommon ) );
		return mCommandList->unmapBuffer( pBuffer );
	}

	bool CommandContext::flushMappedBuffer( GPUBuffer & pBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsCommon ) );
		return mCommandList->flushMappedBuffer( pBuffer );
	}

	bool CommandContext::flushMappedBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsCommon ) );
		return mCommandList->flushMappedBufferRegion( pBuffer, pRegion );
	}

	bool CommandContext::checkCommandListSupport( Bitmask<ECommandListFlags> pCmdListFlags )
	{
		return mCommandList->checkFeatureSupport( pCmdListFlags );
	}


	const Bitmask<ECommandListFlags> CommandContextDirect::sListFlagsDirect = E_COMMAND_LIST_FLAG_EXECUTION_MODE_DIRECT_BIT;
	
	void CommandContextDirect::submit()
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirect ) );
		mCommandSystem->submitContext( *this, CX_COMMAND_CONTEXT_SUBMIT_DEFAULT );
	}

	CommandSync CommandContextDirect::submit( const CommandContextSubmitInfo & pSubmitInfo )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirect ) );
		return mCommandSystem->submitContext( *this, pSubmitInfo );
	}

	void CommandContextDirect::executeDeferredContext( CommandContextDeferred & pDeferredContext )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirect ) );
		return mCommandList->executeDeferredContext( pDeferredContext );
	}

	bool CommandContextDirect::invalidateBuffer( GPUBuffer & pBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirect ) );
		return mCommandList->invalidateBuffer( pBuffer );
	}

	bool CommandContextDirect::invalidateBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirect ) );
		return mCommandList->invalidateBufferRegion( pBuffer, pRegion );
	}


	const Bitmask<ECommandListFlags> CommandContextDirectTransfer::sListFlagsDirectTransfer = E_COMMAND_LIST_FLAG_EXECUTION_MODE_DIRECT_BIT |
	                                                                                         E_COMMAND_LIST_FLAG_COMMAND_CLASS_TRANSFER_BIT;

	bool CommandContextDirectTransfer::updateBufferDataCopy( GPUBuffer & pBuffer, GPUBuffer & pSourceBuffer, const GPUBufferDataCopyDesc & pCopyDesc )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectTransfer ) );
		return mCommandList->updateBufferDataCopy( pBuffer, pSourceBuffer, pCopyDesc );
	}

	bool CommandContextDirectTransfer::updateBufferSubDataCopy( GPUBuffer & pBuffer, GPUBuffer & pSourceBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectTransfer ) );
		return mCommandList->updateBufferSubDataCopy( pBuffer, pSourceBuffer, pCopyDesc );
	}

	bool CommandContextDirectTransfer::updateBufferDataUpload( GPUBuffer & pBuffer, const GPUBufferDataUploadDesc & pUploadDesc )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectTransfer ) );
		return mCommandList->updateBufferDataUpload( pBuffer, pUploadDesc );
	}

	bool CommandContextDirectTransfer::updateBufferSubDataUpload( GPUBuffer & pBuffer, const GPUBufferSubDataUploadDesc & pUploadDesc )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectTransfer ) );
		return mCommandList->updateBufferSubDataUpload( pBuffer, pUploadDesc );
	}


	const Bitmask<ECommandListFlags> CommandContextDirectCompute::sListFlagsDirectCompute = E_COMMAND_LIST_FLAG_EXECUTION_MODE_DIRECT_BIT |
	                                                                                       E_COMMAND_LIST_FLAG_COMMAND_CLASS_COMPUTE_BIT;

	void CommandContextDirectCompute::dispatchCompute( uint32 pThrGroupSizeX, uint32 pThrGroupSizeY, uint32 pThrGroupSizeZ )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectCompute ) );
		return mCommandList->dispatchCompute( pThrGroupSizeX, pThrGroupSizeY, pThrGroupSizeZ );
	}

	void CommandContextDirectCompute::dispatchComputeIndirect( uint32 pIndirectBufferOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectCompute ) );
		return mCommandList->dispatchComputeIndirect( pIndirectBufferOffset );
	}


	const Bitmask<ECommandListFlags> CommandContextDirectGraphics::sListFlagsDirectGraphics = E_COMMAND_LIST_FLAG_EXECUTION_MODE_DIRECT_BIT |
	                                                                                         E_COMMAND_LIST_FLAG_COMMAND_CLASS_GRAPHICS_BIT;

	void CommandContextDirectGraphics::setColorBufferClearValue( const math::RGBAColorR32Norm & pColorClearValue )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setColorBufferClearValue( pColorClearValue );
	}

	void CommandContextDirectGraphics::setDepthBufferClearValue( float pDepthClearValue )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setDepthBufferClearValue( pDepthClearValue );
	}

	void CommandContextDirectGraphics::setStencilBufferClearValue( uint8 pStencilClearValue )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setStencilBufferClearValue( pStencilClearValue );
	}

	void CommandContextDirectGraphics::clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->clearRenderTarget( pAttachmentMask );
	}

	void CommandContextDirectGraphics::setViewport( const ViewportDesc & pViewportDesc )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setViewport( pViewportDesc );
	}

	bool CommandContextDirectGraphics::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setGraphicsPipelineStateObject( pGraphicsPipelineSO );
	}

	bool CommandContextDirectGraphics::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setVertexStreamStateObject( pVertexStreamSO );
	}

	bool CommandContextDirectGraphics::setRenderTargetStateObject( const RenderTargetStateObject & pRenderTargetSO )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setRenderTargetStateObject( pRenderTargetSO );
	}

	bool CommandContextDirectGraphics::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setShaderConstant( pParamRefID, pData );
	}

	bool CommandContextDirectGraphics::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setShaderConstantBuffer( pParamRefID, pConstantBuffer );
	}

	bool CommandContextDirectGraphics::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setShaderTextureImage( pParamRefID, pTexture );
	}

	bool CommandContextDirectGraphics::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setShaderTextureSampler( pParamRefID, pSampler );
	}

	void CommandContextDirectGraphics::drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->drawDirectIndexed( pIndicesNum, pIndicesOffset );
	}

	void CommandContextDirectGraphics::drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->drawDirectIndexedInstanced( pIndicesNumPerInstance, pInstancesNum, pIndicesOffset );
	}

	void CommandContextDirectGraphics::drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->drawDirectNonIndexed( pVerticesNum, pVerticesOffset );
	}

	void CommandContextDirectGraphics::drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->drawDirectNonIndexedInstanced( pVerticesNumPerInstance, pInstancesNum, pVerticesOffset );
	}


	const Bitmask<ECommandListFlags> CommandContextDeferred::sListFlagsDeferred = E_COMMAND_LIST_FLAG_EXECUTION_MODE_DEFERRED_BIT;

	bool CommandContextDeferred::mapBufferDeferred( GPUBuffer & pBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferred ) );
		return mCommandList->mapBuffer( pBuffer, EGPUMemoryMapMode::WriteAppend );
	}

	bool CommandContextDeferred::mapBufferRegionDeferred( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferred ) );
		return mCommandList->mapBufferRegion( pBuffer, pRegion, EGPUMemoryMapMode::WriteAppend );
	}

	bool CommandContextDeferred::unmapBufferDeferred( GPUBuffer & pBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferred ) );
		return mCommandList->unmapBuffer( pBuffer );
	}


	const Bitmask<ECommandListFlags> CommandContextDeferredGraphics::sListFlagsDeferredGraphics = E_COMMAND_LIST_FLAG_EXECUTION_MODE_DEFERRED_BIT |
	                                                                                             E_COMMAND_LIST_FLAG_COMMAND_CLASS_GRAPHICS_BIT;

	void CommandContextDeferredGraphics::setColorBufferClearValue( const math::RGBAColorR32Norm & pColorClearValue )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setColorBufferClearValue( pColorClearValue );
	}

	void CommandContextDeferredGraphics::setDepthBufferClearValue( float pDepthClearValue )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setDepthBufferClearValue( pDepthClearValue );
	}

	void CommandContextDeferredGraphics::setStencilBufferClearValue( uint8 pStencilClearValue )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setStencilBufferClearValue( pStencilClearValue );
	}

	void CommandContextDeferredGraphics::clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->clearRenderTarget( pAttachmentMask );
	}

	void CommandContextDeferredGraphics::setViewport( const ViewportDesc & pViewportDesc )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setViewport( pViewportDesc );
	}

	bool CommandContextDeferredGraphics::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setGraphicsPipelineStateObject( pGraphicsPipelineSO );
	}

	bool CommandContextDeferredGraphics::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setVertexStreamStateObject( pVertexStreamSO );
	}

	bool CommandContextDeferredGraphics::setRenderTargetStateObject( const RenderTargetStateObject & pRenderTargetSO )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setRenderTargetStateObject( pRenderTargetSO );
	}

	bool CommandContextDeferredGraphics::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setShaderConstant( pParamRefID, pData );
	}

	bool CommandContextDeferredGraphics::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setShaderConstantBuffer( pParamRefID, pConstantBuffer );
	}

	bool CommandContextDeferredGraphics::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setShaderTextureImage( pParamRefID, pTexture );
	}

	bool CommandContextDeferredGraphics::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setShaderTextureSampler( pParamRefID, pSampler );
	}

	void CommandContextDeferredGraphics::drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset, EIndexDataFormat pIndexFormat )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->drawDirectIndexed( pIndicesNum, pIndicesOffset );
	}

	void CommandContextDeferredGraphics::drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset, EIndexDataFormat pIndexFormat )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->drawDirectIndexedInstanced( pIndicesNumPerInstance, pInstancesNum, pIndicesOffset );
	}

	void CommandContextDeferredGraphics::drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->drawDirectNonIndexed( pVerticesNum, pVerticesOffset );
	}

	void CommandContextDeferredGraphics::drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->drawDirectNonIndexedInstanced( pVerticesNumPerInstance, pInstancesNum, pVerticesOffset );
	}

}
