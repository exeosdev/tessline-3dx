
#include "commandList.h"
#include "commandSystem.h"
#include "gpuDevice.h"
#include "resources/gpuBuffer.h"
#include "state/graphicsPipelineStateController.h"
#include "state/renderTargetDynamicStates.h"
#include "state/renderTargetImmutableStates.h"

namespace ts3::gpuapi
{

	enum ECommandListInternalStateFlags : uint32
	{
		E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT = 0x10000
	};

	CommandList::CommandList(
			CommandSystem & pCommandSystem,
			ECommandListType pListType,
			GraphicsPipelineStateController & pPipelineStateController )
	: GPUDeviceChildObject( pCommandSystem.mGPUDevice )
	, mCommandSystem( &pCommandSystem )
	, mListType( pListType )
	, _graphicsPipelineStateController( &pPipelineStateController )
	{}

	CommandList::~CommandList() = default;

	bool CommandList::checkCommandClassSupport( ECommandQueueClass pQueueClass ) const noexcept
	{
		return checkFeatureSupport( static_cast<ECommandObjectPropertyFlags>( pQueueClass ) );
	}

	bool CommandList::checkFeatureSupport( Bitmask<ECommandObjectPropertyFlags> pCommandListFlags ) const noexcept
	{
		// Command list type (its value) is basically a bitwise OR of all supported bits.
		Bitmask<ECommandObjectPropertyFlags> commandListPropertyFlags = static_cast<ECommandObjectPropertyFlags>( mListType );

		// Check if the specified command classes and/or execution type matches those supported by the list.
		return commandListPropertyFlags.isSet( pCommandListFlags & E_COMMAND_OBJECT_PROPERTY_MASK_ALL );
	}

	bool CommandList::isRenderPassActive() const noexcept
	{
		return _internalStateMask.isSet( E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT );
	}

	bool CommandList::hasPendingGraphicsPipelineStateChanges() const noexcept
	{
		return _graphicsPipelineStateController->hasPendingStateChanges();
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

	bool CommandList::applyGraphicsPipelineStateChanges()
	{
		return _graphicsPipelineStateController->applyStateChanges();
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
		subDataUploadDesc.bufferRegion.size = getMinOf( pUploadDesc.inputDataDesc.size, pBuffer.mBufferProperties.byteSize );
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

	bool CommandList::beginRenderPass(
			const RenderPassConfigurationImmutableState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags )
	{
		return onBeginRenderPass( pFlags );
	}
	
	bool CommandList::beginRenderPass(
			const RenderPassConfigurationDynamicState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags )
	{
		return onBeginRenderPass( pFlags );
	}

	void CommandList::endRenderPass()
	{
		onEndRenderPass();
	}

	void CommandList::setRenderPassDynamicState( const GraphicsPipelineDynamicState & pDynamicState )
	{
		_graphicsPipelineStateController->setRenderPassDynamicState( pDynamicState );
	}

	bool CommandList::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		return _graphicsPipelineStateController->setGraphicsPipelineStateObject( pGraphicsPSO );
	}

	bool CommandList::setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState )
	{
		return _graphicsPipelineStateController->setIAVertexStreamState( pIAVertexStreamState );
	}

	bool CommandList::setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState )
	{
		return _graphicsPipelineStateController->setIAVertexStreamState( pIAVertexStreamState );
	}

	bool CommandList::setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState )
	{
		return _graphicsPipelineStateController->setRenderTargetBindingState( pRenderTargetBindingState );
	}

	bool CommandList::setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState )
	{
		return _graphicsPipelineStateController->setRenderTargetBindingState( pRenderTargetBindingState );
	}

	bool CommandList::cmdSetViewport( const ViewportDesc & pViewportDesc )
	{
		if( !isRenderPassActive() )
		{
			ts3DebugInterrupt();
			return false;
		}

		return _graphicsPipelineStateController->setViewport( pViewportDesc );
	}

	bool CommandList::cmdSetShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		if( !isRenderPassActive() )
		{
			ts3DebugInterrupt();
			return false;
		}

		return _graphicsPipelineStateController->setShaderConstant( pParamRefID, pData );
	}

	bool CommandList::cmdSetShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		if( !isRenderPassActive() )
		{
			ts3DebugInterrupt();
			return false;
		}

		return _graphicsPipelineStateController->setShaderConstantBuffer( pParamRefID, pConstantBuffer );
	}

	bool CommandList::cmdSetShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		if( !isRenderPassActive() )
		{
			ts3DebugInterrupt();
			return false;
		}

		return _graphicsPipelineStateController->setShaderTextureImage( pParamRefID, pTexture );
	}

	bool CommandList::cmdSetShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		if( !isRenderPassActive() )
		{
			ts3DebugInterrupt();
			return false;
		}

		return _graphicsPipelineStateController->setShaderTextureSampler( pParamRefID, pSampler );
	}

	bool CommandList::onBeginRenderPass( Bitmask<ECommandListActionFlags> pFlags )
	{
		if( isRenderPassActive() )
		{
			return false;
		}

		_internalStateMask.set( E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT );

		_internalStateMask.setOrUnset(
				E_COMMAND_LIST_ACTION_FLAG_RENDER_PASS_PRESERVE_DYNAMIC_STATE_BIT,
				pFlags.isSet( E_COMMAND_LIST_ACTION_FLAG_RENDER_PASS_PRESERVE_DYNAMIC_STATE_BIT ) );

		if( pFlags.isSet( E_COMMAND_LIST_ACTION_FLAG_RENDER_PASS_APPLY_PIPELINE_STATE_BIT ) )
		{
			_graphicsPipelineStateController->applyStateChanges();
		}

		return true;
	}

	void CommandList::onEndRenderPass()
	{
		ts3DebugAssert( isRenderPassActive() );

		if( _internalStateMask.isSet( E_COMMAND_LIST_ACTION_FLAG_RENDER_PASS_PRESERVE_DYNAMIC_STATE_BIT ) )
		{
			_graphicsPipelineStateController->resetRenderPassDynamicState();
		}

		_internalStateMask.unset( E_COMMAND_LIST_INTERNAL_STATE_FLAG_ACTIVE_RENDER_PASS_BIT );
	}


	CommandListRenderPassDefault::CommandListRenderPassDefault(
			CommandSystem & pCommandSystem,
			ECommandListType pListType,
			GraphicsPipelineStateController & pPipelineStateController )
	: CommandList( pCommandSystem, pListType, pPipelineStateController )
	{}

	CommandListRenderPassDefault::~CommandListRenderPassDefault() = default;

	const RenderPassConfiguration & CommandListRenderPassDefault::getRenderPassConfiguration() const noexcept
	{
		return _currentRenderPassConfiguration;
	}

	bool CommandListRenderPassDefault::beginRenderPass(
			const RenderPassConfigurationImmutableState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags )
	{
		if( CommandList::beginRenderPass( pRenderPassState, pFlags ) )
		{
			const auto * defaultRenderPassState = pRenderPassState.queryInterface<RenderPassConfigurationImmutableStateDefault>();
			_currentRenderPassConfiguration = defaultRenderPassState->mRenderPassConfiguration;

			executeRenderPassLoadActions( _currentRenderPassConfiguration );

			return true;
		}

		return false;
	}

	bool CommandListRenderPassDefault::beginRenderPass(
			const RenderPassConfigurationDynamicState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags )
	{
		if( CommandList::beginRenderPass( pRenderPassState, pFlags ) )
		{
			_currentRenderPassConfiguration = pRenderPassState.getRenderPassConfiguration();

			executeRenderPassLoadActions( _currentRenderPassConfiguration );

			return true;
		}

		return false;
	}

	void CommandListRenderPassDefault::endRenderPass()
	{
		if( isRenderPassActive() )
		{
			executeRenderPassStoreActions( _currentRenderPassConfiguration );

			CommandList::endRenderPass();
		}
	}


} // namespace ts3::gpuapi
