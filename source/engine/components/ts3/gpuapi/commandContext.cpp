
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

	void CommandContextDirect::cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirect ) );
		return mCommandList->cmdExecuteDeferredContext( pDeferredContext );
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


	const Bitmask<ECommandListFlags> CommandContextDirectTransfer::sListFlagsDirectTransfer =
			E_COMMAND_LIST_FLAG_EXECUTION_MODE_DIRECT_BIT | E_COMMAND_LIST_FLAG_COMMAND_CLASS_TRANSFER_BIT;

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


	const Bitmask<ECommandListFlags> CommandContextDirectCompute::sListFlagsDirectCompute =
			E_COMMAND_LIST_FLAG_EXECUTION_MODE_DIRECT_BIT | E_COMMAND_LIST_FLAG_COMMAND_CLASS_COMPUTE_BIT;

	void CommandContextDirectCompute::cmdDispatchCompute( uint32 pThrGroupSizeX, uint32 pThrGroupSizeY, uint32 pThrGroupSizeZ )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectCompute ) );
		return mCommandList->cmdDispatchCompute( pThrGroupSizeX, pThrGroupSizeY, pThrGroupSizeZ );
	}

	void CommandContextDirectCompute::cmdDispatchComputeIndirect( uint32 pIndirectBufferOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectCompute ) );
		return mCommandList->cmdDispatchComputeIndirect( pIndirectBufferOffset );
	}


	const Bitmask<ECommandListFlags> CommandContextDirectGraphics::sListFlagsDirectGraphics =
			E_COMMAND_LIST_FLAG_EXECUTION_MODE_DIRECT_BIT | E_COMMAND_LIST_FLAG_COMMAND_CLASS_GRAPHICS_BIT;

	bool CommandContextDirectGraphics::cmdSetBlendConstantColor( const math::RGBAColorR32Norm & pColor )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdSetBlendConstantColor( pColor );
	}

	bool CommandContextDirectGraphics::cmdSetViewport( const ViewportDesc & pViewportDesc )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdSetViewport( pViewportDesc );
	}

	bool CommandContextDirectGraphics::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->setGraphicsPipelineStateObject( pGraphicsPSO );
	}

	bool CommandContextDirectGraphics::cmdSetShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdSetShaderConstant( pParamRefID, pData );
	}

	bool CommandContextDirectGraphics::cmdSetShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdSetShaderConstantBuffer( pParamRefID, pConstantBuffer );
	}

	bool CommandContextDirectGraphics::cmdSetShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdSetShaderTextureImage( pParamRefID, pTexture );
	}

	bool CommandContextDirectGraphics::cmdSetShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdSetShaderTextureSampler( pParamRefID, pSampler );
	}

	void CommandContextDirectGraphics::cmdDrawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdDrawDirectIndexed( pIndicesNum, pIndicesOffset );
	}

	void CommandContextDirectGraphics::cmdDrawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdDrawDirectIndexedInstanced( pIndicesNumPerInstance, pInstancesNum, pIndicesOffset );
	}

	void CommandContextDirectGraphics::cmdDrawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdDrawDirectNonIndexed( pVerticesNum, pVerticesOffset );
	}

	void CommandContextDirectGraphics::cmdDrawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDirectGraphics ) );
		return mCommandList->cmdDrawDirectNonIndexedInstanced( pVerticesNumPerInstance, pInstancesNum, pVerticesOffset );
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


	const Bitmask<ECommandListFlags> CommandContextDeferredGraphics::sListFlagsDeferredGraphics =
			E_COMMAND_LIST_FLAG_EXECUTION_MODE_DEFERRED_BIT | E_COMMAND_LIST_FLAG_COMMAND_CLASS_GRAPHICS_BIT;

	bool CommandContextDeferredGraphics::cmdSetBlendConstantColor( const math::RGBAColorR32Norm & pColor )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdSetBlendConstantColor( pColor );
	}

	bool CommandContextDeferredGraphics::cmdSetViewport( const ViewportDesc & pViewportDesc )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdSetViewport( pViewportDesc );
	}

	bool CommandContextDeferredGraphics::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->setGraphicsPipelineStateObject( pGraphicsPSO );
	}

	bool CommandContextDeferredGraphics::cmdSetShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdSetShaderConstant( pParamRefID, pData );
	}

	bool CommandContextDeferredGraphics::cmdSetShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdSetShaderConstantBuffer( pParamRefID, pConstantBuffer );
	}

	bool CommandContextDeferredGraphics::cmdSetShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdSetShaderTextureImage( pParamRefID, pTexture );
	}

	bool CommandContextDeferredGraphics::cmdSetShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdSetShaderTextureSampler( pParamRefID, pSampler );
	}

	void CommandContextDeferredGraphics::cmdDrawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset, EIndexDataFormat pIndexFormat )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdDrawDirectIndexed( pIndicesNum, pIndicesOffset );
	}

	void CommandContextDeferredGraphics::cmdDrawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset, EIndexDataFormat pIndexFormat )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdDrawDirectIndexedInstanced( pIndicesNumPerInstance, pInstancesNum, pIndicesOffset );
	}

	void CommandContextDeferredGraphics::cmdDrawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdDrawDirectNonIndexed( pVerticesNum, pVerticesOffset );
	}

	void CommandContextDeferredGraphics::cmdDrawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset )
	{
		ts3DebugAssert( checkCommandListSupport( sListFlagsDeferredGraphics ) );
		return mCommandList->cmdDrawDirectNonIndexedInstanced( pVerticesNumPerInstance, pInstancesNum, pVerticesOffset );
	}

} // namespace ts3::gpuapi
