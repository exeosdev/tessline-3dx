
#include "commandList.h"
#include "commandSystem.h"
#include "gpuDevice.h"
#include "resources/gpuBuffer.h"
#include "state/graphicsPipelineStateController.h"

namespace ts3::gpuapi
{

	enum ECommandListInternalStateFlags : uint32
	{
		E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT = 0x01
	};

	CommandList::CommandList(
			CommandSystem & pCommandSystem,
			ECommandListType pListType,
			GraphicsPipelineStateController & pPipelineStateController )
	: GPUDeviceChildObject( pCommandSystem.mGPUDevice )
	, mCommandSystem( &pCommandSystem )
	, mListType( pListType )
	, _pipelineStateController( &pPipelineStateController )
	{}

	CommandList::~CommandList() = default;

	bool CommandList::checkContextSupport( ECommandContextType pContextType ) const noexcept
	{
		return checkFeatureSupport( static_cast<ECommandListFlags>( pContextType ) );
	}

	bool CommandList::checkFeatureSupport( Bitmask<ECommandListFlags> pListFlags ) const noexcept
	{
		// Command list type (its value) is basically a bitwise OR of all supported bits.
		Bitmask<ECommandListFlags> commandListFlags = static_cast<ECommandListFlags>( mListType );
		// Check if the specified command classes and/or execution type matches those supported by the list.
		return commandListFlags.isSet( pListFlags & E_COMMAND_LIST_FLAGS_ALL );
	}

	bool CommandList::isRenderPassActive() const noexcept
	{
		return _internalStateMask.isSet( E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT );
	}

	bool CommandList::acquireList()
	{
		auto listLockStatus = false;
		auto acquireSuccessful = _listLockStatus.compare_exchange_strong(
			listLockStatus,
			true,
			std::memory_order_acq_rel,
			std::memory_order_relaxed );

		return acquireSuccessful;
	}

	void CommandList::releaseList()
	{
		_listLockStatus.store( false, std::memory_order_release );
	}

	void CommandList::beginCommandSequence()
	{}

	void CommandList::endCommandSequence()
	{}

	bool CommandList::mapBuffer( GPUBuffer & pBuffer, EGPUMemoryMapMode pMapMode )
	{
		return mapBufferRegion( pBuffer, GPUMemoryRegion{ 0, pBuffer.mBufferProperties.byteSize }, pMapMode );
	}

	bool CommandList::mapBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode )
	{
		if( !pBuffer.validateMapRequest( pRegion, pMapMode ) )
		{
			ts3DebugInterrupt();
			return false;
		}

		return pBuffer.mapRegion( this, pRegion, pMapMode );
	}

	bool CommandList::unmapBuffer( GPUBuffer & pBuffer )
	{
		if( !pBuffer.isMapped() )
		{
			ts3DebugInterrupt();
			return false;
		}

		pBuffer.unmap( this );

		return true;
	}

	bool CommandList::flushMappedBuffer( GPUBuffer & pBuffer )
	{
		GPUMemoryRegion flushRegion;
		flushRegion.offset = 0;
		flushRegion.size = pBuffer.mBufferProperties.byteSize;

		return flushMappedBufferRegion( pBuffer, flushRegion );
	}

	bool CommandList::flushMappedBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion )
	{
		if( !pBuffer.isMapped() )
		{
			ts3DebugInterrupt();
			return false;
		}

		pBuffer.flushMappedRegion( this, pRegion );

		return true;
	}

	bool CommandList::invalidateBuffer( GPUBuffer & pBuffer )
	{
		GPUMemoryRegion invalidateRegion;
		invalidateRegion.offset = 0;
		invalidateRegion.size = pBuffer.mBufferProperties.byteSize;

		return invalidateBufferRegion( pBuffer, invalidateRegion );
	}

	bool CommandList::invalidateBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion )
	{
		pBuffer.invalidateRegion( this, pRegion );
		return true;
	}

	bool CommandList::updateBufferDataCopy( GPUBuffer & pBuffer, GPUBuffer & pSourceBuffer, const GPUBufferDataCopyDesc & pCopyDesc )
	{
		GPUBufferSubDataCopyDesc subDataCopyDesc;
		subDataCopyDesc.flags = pCopyDesc.flags;
		subDataCopyDesc.flags.set( E_GPU_BUFFER_DATA_COPY_FLAG_MODE_INVALIDATE_BIT );
		subDataCopyDesc.flags.unset( E_GPU_BUFFER_DATA_COPY_FLAG_MODE_APPEND_BIT );
		subDataCopyDesc.sourceBufferRegion.offset = 0;
		subDataCopyDesc.sourceBufferRegion.size = pSourceBuffer.mBufferProperties.byteSize;
		subDataCopyDesc.targetBufferOffset = 0;

		return updateBufferSubDataCopy( pBuffer, pSourceBuffer, subDataCopyDesc );
	}

	bool CommandList::updateBufferSubDataCopy( GPUBuffer & pBuffer, GPUBuffer & pSourceBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc )
	{
		if( pBuffer.isMapped() || pSourceBuffer.isMapped() )
		{
			ts3DebugInterrupt();
			return false;
		}

		if( pBuffer.mBufferProperties.byteSize != pSourceBuffer.mBufferProperties.byteSize )
		{
			ts3DebugInterrupt();
			return false;
		}

		pBuffer.updateSubDataCopy( this, pSourceBuffer, pCopyDesc );

		return true;
	}

	bool CommandList::updateBufferDataUpload( GPUBuffer & pBuffer, const GPUBufferDataUploadDesc & pUploadDesc )
	{
		GPUBufferSubDataUploadDesc subDataUploadDesc;
		subDataUploadDesc.flags = pUploadDesc.flags;
		subDataUploadDesc.flags.set( E_GPU_BUFFER_DATA_COPY_FLAG_MODE_INVALIDATE_BIT );
		subDataUploadDesc.flags.unset( E_GPU_BUFFER_DATA_COPY_FLAG_MODE_APPEND_BIT );
		subDataUploadDesc.bufferRegion.offset = 0;
		subDataUploadDesc.bufferRegion.size = pBuffer.mBufferProperties.byteSize;
		subDataUploadDesc.inputDataDesc = pUploadDesc.inputDataDesc;

		return updateBufferSubDataUpload( pBuffer, subDataUploadDesc );
	}

	bool CommandList::updateBufferSubDataUpload( GPUBuffer & pBuffer, const GPUBufferSubDataUploadDesc & pUploadDesc )
	{
		if( !pUploadDesc.inputDataDesc )
		{
			ts3DebugInterrupt();
			return false;
		}

		if( pBuffer.isMapped() )
		{
			ts3DebugInterrupt();
			return false;
		}

		pBuffer.updateSubDataUpload( this, pUploadDesc );

		return true;
	}

	bool CommandList::beginRenderPass( const RenderPassConfigurationImmutableState & pRenderPassState )
	{
		if( isRenderPassActive() )
		{
			return false;
		}
		_internalStateMask.set( E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT );
		_pipelineStateController->applyPipelineStateChanges();
		return true;
	}
	
	bool CommandList::beginRenderPass( const RenderPassConfigurationDynamicState & pRenderPassState )
	{
		if( isRenderPassActive() )
		{
			return false;
		}
		_internalStateMask.set( E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT );
		_pipelineStateController->applyPipelineStateChanges();
		return true;
	}

	void CommandList::endRenderPass()
	{
		_internalStateMask.unset( E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT );
	}

	bool CommandList::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		return _pipelineStateController->setGraphicsPipelineStateObject( pGraphicsPSO );
	}

	bool CommandList::setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState )
	{
		return _pipelineStateController->setIAVertexStreamState( pIAVertexStreamState );
	}

	bool CommandList::setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState )
	{
		return _pipelineStateController->setIAVertexStreamState( pIAVertexStreamState );
	}

	bool CommandList::setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState )
	{
		return _pipelineStateController->setRenderTargetBindingState( pRenderTargetBindingState );
	}

	bool CommandList::setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState )
	{
		return _pipelineStateController->setRenderTargetBindingState( pRenderTargetBindingState );
	}

} // namespace ts3::gpuapi
